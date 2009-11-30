/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: urp_environment.cxx,v $
 * $Revision: 1.21.20.2 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_bridges.hxx"

#include <stdio.h>

#include <osl/interlck.h>
#include <osl/diagnose.h>
#include <osl/conditn.h>
#include <osl/mutex.hxx>
#include <osl/process.h>

#include <rtl/alloc.h>
#include <rtl/uuid.h>
#include <rtl/unload.h>

#include <uno/environment.h>
#include <uno/lbnames.h>
#include <uno/mapping.hxx>
#include <uno/threadpool.h>

#include <com/sun/star/uno/Sequence.hxx>

#include <bridges/remote/proxy.hxx>
#include <bridges/remote/stub.hxx>
#include <bridges/remote/context.h>
#include <bridges/remote/mapping.hxx>
#include <bridges/remote/counter.hxx>
#include <bridges/remote/bridgeimpl.hxx>
#include <bridges/remote/helper.hxx>

#include "urp_bridgeimpl.hxx"
#include "urp_writer.hxx"
#include "urp_reader.hxx"
#include "urp_dispatch.hxx"
#include "urp_log.hxx"
#include "urp_propertyobject.hxx"

using namespace ::rtl;
using namespace ::osl;
using namespace ::com::sun::star::uno;

namespace bridges_urp
{
rtl_StandardModuleCount g_moduleCount = MODULE_COUNT_INIT;

//  static void dumpProperties( struct Properties *p )
//  {
//  	fprintf( stderr , "FlushBlockSize     : %d\n" , p->nFlushBlockSize );
//  	fprintf( stderr , "OnewayTimeoutMUSEC : %d\n" , p->nOnewayTimeoutMUSEC );
//  	fprintf( stderr , "OidCacheSize       : %d\n" , p->nOidCacheSize );
//  	fprintf( stderr , "TypeCacheSize      : %d\n" , p->nTypeCacheSize );
//  	fprintf( stderr , "TidCacheSize       : %d\n" , p->nTidCacheSize );
//  	OString o = OUStringToOString( p->sSupportedVersions , RTL_TEXTENCODING_ASCII_US );
//  	fprintf( stderr , "SupportedVersions  : %s\n" , o.pData->buffer );
//  	o = OUStringToOString( p->sVersion , RTL_TEXTENCODING_ASCII_US );
//  	fprintf( stderr , "Version : %s\n" , o.pData->buffer );
//  	fprintf( stderr , "SupportsMultipleSynchronous : %d\n" , p->bSupportsMultipleSynchronous );
//  	fprintf( stderr , "SupportsMustReply   : %d\n" , p->bSupportsMustReply );
//  	fprintf( stderr , "SupportsSynchronous : %d\n" , p->bSupportsSynchronous );
//  }

// PropertySetterThread
//------------------------------------
class PropertySetterThread : public ::osl::Thread
{
    urp_BridgeImpl *m_pImpl;
    ::rtl::OUString m_sProps;
    uno_Environment *m_pEnvRemote;
public:
    PropertySetterThread( uno_Environment *pEnvRemote,
                          urp_BridgeImpl *pImpl,
                          const ::rtl::OUString  & props )
        : m_pImpl( pImpl )
        , m_sProps( props )
        , m_pEnvRemote( pEnvRemote )
        {
            if (m_sProps.getLength() > 0) {
                m_sProps += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(","));
            }
            m_sProps += rtl::OUString(
                RTL_CONSTASCII_USTRINGPARAM("CurrentContext="));
            // hold the environment in case all references are released before this
            // thread terminates
            m_pEnvRemote->acquire( pEnvRemote );
        }
    ~PropertySetterThread()
        {
            m_pEnvRemote->release( m_pEnvRemote );
        }

    virtual void SAL_CALL run()
        {
            for (;;)
            {
                switch ( m_pImpl->m_pPropertyObject->localRequestChange( ) )
                {
                case 1:
                    sal_Bool bExceptionThrown;
                    m_pImpl->m_pPropertyObject->localCommitChange( m_sProps , &bExceptionThrown );
                    OSL_ENSURE( !bExceptionThrown, "properties were not set\n" );
                    goto done;
                case 0:
                    OSL_TRACE( "urp-bridge : remote-counterpart won the changing-the-protocol-race\n" );
                    goto done;
                }
            }
        done:
            m_pImpl->m_initialized.set();
        }
    virtual void SAL_CALL onTerminated()
        {
            delete this;
        }
};
//------------------------------------


void test_cache()
{
    OUString a = OUString( RTL_CONSTASCII_USTRINGPARAM( "a" ) );
    OUString b = OUString( RTL_CONSTASCII_USTRINGPARAM( "b" ) );
    OUString c = OUString( RTL_CONSTASCII_USTRINGPARAM( "c" ) );
    Cache < OUString , equalOUString > cache ( 2 );

    sal_Int32 n = cache.put( a );
    if (cache.seek( a ) != n)
    {
        OSL_ASSERT( false );
    }
    OSL_ASSERT( 0 == n );

    n = cache.put( b );
    OSL_ASSERT( 1 == n );

    cache.put( c );

    OSL_ASSERT( 0xffff == cache.seek( a ) );
    OSL_ASSERT( 1 == cache.seek( b ) );
    OSL_ASSERT( 0 == cache.seek( c ) );

    OSL_ASSERT( 1 == cache.put( a ) );
    OSL_ASSERT( 0xffff == cache.seek( b) );
    OSL_ASSERT( 1 == cache.seek( a ) );
    OSL_ASSERT( 0 == cache.seek( c ) );
}

/*******************
 * Are we within thread, that calls destructors of static objects ?
 *******************/
sal_Bool g_bStaticDestructorsCalled = sal_False;
struct StaticSingleton
{
    ~StaticSingleton()
        {
            ::osl::MutexGuard guard( ::osl::Mutex::getGlobalMutex() );
            g_bStaticDestructorsCalled = sal_True;
        }
};
StaticSingleton singleton;

#if OSL_DEBUG_LEVEL > 1
static MyCounter thisCounter( "Remote Environment" );
#endif

void SAL_CALL allThreadsAreGone( uno_Environment * pEnvRemote )
{
    remote_Context *pContext = (remote_Context *) pEnvRemote->pContext;
    urp_BridgeImpl *pImpl = ( urp_BridgeImpl *) pContext->m_pBridgeImpl;

    // if the current thread is not the writer thread, the writer thread
    // object is not destroyed up to now, though it may already have run out.
    // In both cases, it must be safe to cal pImpl->m_pWriter->getIdentifier()
    OSL_ASSERT( pImpl->m_pWriter );
    if( pImpl->m_pWriter->getIdentifier() == osl_getThreadIdentifier(0) )
    {
        // This is the writer thread. It has done some release calls,
        // and is now the last one, that was active. Because the writer
        // thread holds the environment weakly, there may also be a thread within
        // the dispose of the bridge ( because the enviroment may have a refcount == 0 ).
        // However, this thread MUST wait for the writer thread, so it is perfectly ok,
        // not to set m_cndWaitForThreads. ( The check for m_nRemoteThreads is done
        // after the join of the writer thread ).
    }
    else
    {
        pImpl->m_cndWaitForThreads.set();
    }

}

void SAL_CALL releaseStubs( uno_Environment *pEnvRemote )
{

    ((remote_Context * ) pEnvRemote->pContext )->m_pBridgeImpl->m_bReleaseStubsCalled = sal_True;

    remote_Interface **ppInterfaces = 0;
    sal_Int32 nCount;
    pEnvRemote->pExtEnv->getRegisteredInterfaces( pEnvRemote->pExtEnv,
                                                  (void***)&ppInterfaces,
                                                  &nCount,
                                                  rtl_allocateMemory );

    sal_Int32 i;
    for( i  = 0 ; i < nCount ; i ++ )
    {
          if( ppInterfaces[i]->acquire == bridges_remote::acquireUno2RemoteStub )
          {
              // these are freed by the environment, so no release necessary
              pEnvRemote->pExtEnv->revokeInterface( pEnvRemote->pExtEnv, ppInterfaces[i] );
          }
          else
          {
            ppInterfaces[i]->release( ppInterfaces[i] );
          }
    }

    rtl_freeMemory( (void*) ppInterfaces );
}

} // end namespace bridges_urp

using namespace bridges_urp;

extern "C" {

static void SAL_CALL RemoteEnvironment_thisDispose( uno_Environment *pEnvRemote )
{
    remote_Context *pContext = (remote_Context *) pEnvRemote->pContext;
    urp_BridgeImpl *pImpl = ( urp_BridgeImpl *) pContext->m_pBridgeImpl;

    ::osl::ClearableMutexGuard guard( pImpl->m_disposingMutex  );
      if( pContext->m_pBridgeImpl->m_bDisposed &&
          ( ! pImpl->m_pReader ||
            osl_getThreadIdentifier(0) ==
            (oslThreadIdentifier) pImpl->m_pReader->getIdentifier() ) )
      {
          return;
      }
    // in case, that the static destructors already have been called, no
    // tiding up is done.
    bool tidyUp;
    {
        ::osl::MutexGuard guard2( ::osl::Mutex::getGlobalMutex() );
        tidyUp = ! g_bStaticDestructorsCalled &&
            ! pContext->m_pBridgeImpl->m_bDisposed;
    }
    if( tidyUp )
    {
        // TODO : not threadsafe
        // synchronization with dispatch methods needed !

        pImpl->m_bDisposed = sal_True;


        // close the connection
        uno_threadpool_dispose( pImpl->m_hThreadPool );
        pImpl->m_pWriter->abortThread();
          pContext->m_pConnection->close( pContext->m_pConnection );

        if( osl_getThreadIdentifier(0) ==
            (oslThreadIdentifier) pImpl->m_pReader->getIdentifier() )
        {
            // This is the reader thread. Let the thread destroy itself
            // the reader thread object must also release the connection at this stage !
            pImpl->m_pReader->destroyYourself();
            pImpl->m_pReader = 0;
        }
        else
        {
            // wait for the reader thread
            // the reader thread object must also release the connection,
            // when terminating !!!!
            pImpl->m_pReader->join();
        }

        // wait for the writer thread
        pImpl->m_pWriter->join();

        // now let the context go !
        pContext->dispose( pContext );

          if( 0 != pImpl->m_nRemoteThreads )
          {
              // Wait for all threads
            guard.clear();
              pImpl->m_cndWaitForThreads.wait();
              OSL_ASSERT( ! pImpl->m_nRemoteThreads );
          }
          else
          {
              guard.clear();
          }
#ifdef BRIDGES_URP_PROT
        if( pImpl->m_pLogFile )
        {
            fclose( pImpl->m_pLogFile );
            pImpl->m_pLogFile = 0;
        }
#endif
#if OSL_DEBUG_LEVEL > 1
        pImpl->dumpErrors( stderr );
#endif

        // destroy the threads
        delete pImpl->m_pWriter;
        pImpl->m_pWriter = 0;

        if( pImpl->m_pReader != 0 )
        {
            // This is not the reader thread, so the thread object is deleted
            delete pImpl->m_pReader;
            pImpl->m_pReader = 0;
        }

        ::osl::MutexGuard guard2( ::osl::Mutex::getGlobalMutex() );
        if( ! g_bStaticDestructorsCalled )
        {
            // delete the stubs
            releaseStubs( pEnvRemote );
        }
    }
}

static void SAL_CALL RemoteEnvironment_thisDisposing(
    uno_Environment *pEnvRemote )
{
    remote_Context *pContext = (remote_Context * )pEnvRemote->pContext;
    urp_BridgeImpl *pImpl = ((urp_BridgeImpl*) pContext->m_pBridgeImpl);

    {
        ::osl::ClearableMutexGuard guard( pImpl->m_disposingMutex );
        if( ! pImpl->m_bDisposed )
        {
            guard.clear();
            urp_sendCloseConnection( pEnvRemote );
            RemoteEnvironment_thisDispose( pEnvRemote );
        }
    }
    pImpl->m_pPropertyObject->thisRelease();
    pImpl->m_pPropertyObject = 0;

     uno_threadpool_destroy( pImpl->m_hThreadPool );

    delete pImpl;
    pContext->aBase.release( (uno_Context * ) pContext );
    g_moduleCount.modCnt.release( &g_moduleCount.modCnt );
#if OSL_DEBUG_LEVEL > 1
      thisCounter.release();
#endif
}

static void SAL_CALL RemoteEnvironment_thisComputeObjectIdentifier(
    uno_ExtEnvironment *, rtl_uString **, void *)
{
    OSL_ENSURE( 0, "RemoteEnvironment_thisComputeObjectIdentifier should never be called" );
}

static void SAL_CALL RemoteEnvironment_thisAcquireInterface(
    uno_ExtEnvironment *, void *pInterface )
{
    ((remote_Interface *)pInterface)->acquire( ( remote_Interface *) pInterface );
}

static void SAL_CALL RemoteEnvironment_thisReleaseInterface(
    uno_ExtEnvironment *, void *pInterface )
{
    ((remote_Interface *)pInterface)->release( ( remote_Interface *) pInterface );
}

//##################################################################################################
void SAL_CALL uno_initEnvironment( uno_Environment * pEnvRemote )
{
    g_moduleCount.modCnt.acquire( &g_moduleCount.modCnt );
    // set C-virtual methods
    pEnvRemote->environmentDisposing = RemoteEnvironment_thisDisposing;
    pEnvRemote->pExtEnv->computeObjectIdentifier = RemoteEnvironment_thisComputeObjectIdentifier;
    pEnvRemote->pExtEnv->acquireInterface = RemoteEnvironment_thisAcquireInterface;
    pEnvRemote->pExtEnv->releaseInterface = RemoteEnvironment_thisReleaseInterface;
    pEnvRemote->dispose = RemoteEnvironment_thisDispose;

    remote_Context *pContext = ( remote_Context * ) pEnvRemote->pContext;
    pContext->aBase.acquire( ( uno_Context * )  pContext );
      pContext->getRemoteInstance = ::bridges_remote::remote_sendQueryInterface;

    // Initialize impl struct     urp_BridgeImpl
    urp_BridgeImpl *pImpl = new ::bridges_urp::urp_BridgeImpl( 256, 8192 );
    pContext->m_pBridgeImpl = pImpl;

    // Initialize threadpool
    pImpl->m_hThreadPool = uno_threadpool_create();

    // take the bridgepointer as id
    pImpl->m_properties.seqBridgeID = ByteSequence( (sal_Int8*)&pEnvRemote , sizeof( pEnvRemote ) );

    pImpl->m_allThreadsAreGone = allThreadsAreGone;
    pImpl->m_sendRequest = urp_sendRequest;
    pImpl->m_nRemoteThreads = 0;
    pImpl->m_bDisposed = sal_False;
    pImpl->m_bReleaseStubsCalled = sal_False;

    pImpl->m_pPropertyObject = new PropertyObject( &(pImpl->m_properties ),  pEnvRemote, pImpl );
    pImpl->m_pPropertyObject->thisAcquire();

    OUString sProtocolProperties;
    if( pContext->m_pProtocol->length > 3 )
    {
        sProtocolProperties = OUString( pContext->m_pProtocol ).copy( 4, pContext->m_pProtocol->length-4);
    }
    if( sProtocolProperties.getLength() )
    {
        struct Properties props = pImpl->m_properties;
        assignFromStringToStruct( sProtocolProperties , &props );
        if( ! props.bNegotiate )
        {
            // no negotiation takes place, the creator of the bridge knows the parameter
            // of the other side !
            pImpl->applyProtocolChanges( props );
            sProtocolProperties = OUString();
        }
    }

#ifdef BRIDGES_URP_PROT
      char *p = getenv( "PROT_REMOTE" );
    pImpl->m_pLogFile = 0;
      if( p )
      {
        MutexGuard guard( Mutex::getGlobalMutex() );
        static int counter;
        oslProcessInfo data;
        data.Size = sizeof( data );
        osl_getProcessInfo( 0 , osl_Process_HEAPUSAGE | osl_Process_IDENTIFIER , &data );
        OString s(p);
        s += "_pid";
        s += OString::valueOf( (sal_Int32) data.Ident );
        s += "_";
        s += OString::valueOf( (sal_Int32) counter );
        pImpl->m_sLogFileName = s;
        // clear the file
          FILE *f = fopen( s.getStr() , "w" );
        OSL_ASSERT( f );
        if( getenv( "PROT_REMOTE_FAST") )
        {
            pImpl->m_pLogFile = f;
        }
        else
        {
            fclose( f );
        }
        counter++;
      }
#endif

    // start reader and writer threads
    pImpl->m_pWriter = new ::bridges_urp::OWriterThread( pContext->m_pConnection , pImpl,
                                                         pEnvRemote);
    pImpl->m_pWriter->create();

    pImpl->m_pReader = new ::bridges_urp::OReaderThread( pContext->m_pConnection ,
                                                         pEnvRemote,
                                                         pImpl->m_pWriter );
    pImpl->m_pReader->create();

    PropertySetterThread *pPropsSetterThread =
        new PropertySetterThread( pEnvRemote, pImpl , sProtocolProperties );
    pPropsSetterThread->create();
#if OSL_DEBUG_LEVEL > 1
    thisCounter.acquire();
#endif
}


//##################################################################################################
void SAL_CALL uno_ext_getMapping(
    uno_Mapping ** ppMapping,
    uno_Environment * pFrom,
    uno_Environment * pTo )
{
    OSL_ASSERT( ppMapping && pFrom && pTo );
    if (ppMapping && pFrom && pTo)
    {
        if (*ppMapping)
            ((*ppMapping)->release)( *ppMapping );
        bridges_remote::RemoteMapping * pMapping = 0;

        ::rtl::OUString sFromName = pFrom->pTypeName;
        ::rtl::OUString sToName = pTo->pTypeName;
        ::rtl::OUString sUno = OUString::createFromAscii( UNO_LB_UNO );
        ::rtl::OUString sRemote = OUString::createFromAscii( "urp" );
        if ( sFromName.equalsIgnoreAsciiCase( sRemote ) &&
             sToName.equalsIgnoreAsciiCase( sUno ) )
        {
            pMapping =  new bridges_remote::RemoteMapping( pTo, /* Uno */
                                     pFrom, /*remote*/
                                     bridges_remote::remoteToUno,
                                     OUString() );
        }
        else if ( sFromName.equalsIgnoreAsciiCase( sUno ) &&
                  sToName.equalsIgnoreAsciiCase( sRemote ) )
        {
            pMapping =  new bridges_remote::RemoteMapping( pFrom ,
                                           pTo ,
                                           bridges_remote::unoToRemote,
                                           OUString() );
        }

        *ppMapping = (uno_Mapping * )pMapping;
        OUString dummy;
        uno_registerMapping( ppMapping ,
                             bridges_remote::freeRemoteMapping,
                             pFrom ,
                             pTo ,
                             dummy.pData );
    }
}

sal_Bool SAL_CALL component_canUnload( TimeValue *pTime )
{
    return g_moduleCount.canUnload( &g_moduleCount , pTime );
}

}
