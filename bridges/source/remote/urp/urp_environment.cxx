/*************************************************************************
 *
 *  $RCSfile: urp_environment.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: jbu $ $Date: 2001-05-14 09:57:58 $
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

#include <assert.h>
#include <stdio.h>

#include <osl/interlck.h>
#include <osl/diagnose.h>
#include <osl/conditn.h>
#include <osl/mutex.hxx>
#include <osl/process.h>

#include <rtl/alloc.h>
#include <rtl/uuid.h>

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
            struct Properties props;
            if( m_sProps.getLength() )
            {
                sal_Int32 nResult = m_pImpl->m_pPropertyObject->localRequestChange( );
                if( 1  == nResult )
                {
                    sal_Bool bExceptionThrown;
                    m_pImpl->m_pPropertyObject->localCommitChange( m_sProps , &bExceptionThrown );
                    OSL_ENSURE( !bExceptionThrown, "properties were not set\n" );
                }
                else if( 0 == nResult )
                {
                    OSL_TRACE( "urp-bridge : remote-counterpart won the changing-the-protocol-race\n" );
                }
                else
                {
                    OSL_ASSERT( !"urp-bridge : property setting failed because identical random numbers " );
                }
            }
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
    sal_Int32 nR = cache.seek( a );
    OSL_ASSERT( n == nR );
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
            g_bStaticDestructorsCalled = sal_True;
        }
};
StaticSingleton singleton;

#ifdef DEBUG
static MyCounter thisCounter( "Remote Environment" );
#endif

struct RemoteEnvironment
{
    
    static void SAL_CALL thisDisposing( uno_Environment *pEnvRemote );
    static void SAL_CALL thisComputeObjectIdentifier( uno_ExtEnvironment *pEnvRemote,
                                                      rtl_uString **ppOid,
                                                      void *pInterface );
    static void SAL_CALL thisAcquireInterface( uno_ExtEnvironment * pEnvRemote ,
                                               void *pInterface );
    static void SAL_CALL thisReleaseInterface( uno_ExtEnvironment * pEnvRemote,
                                               void *pInterface);
    static void SAL_CALL thisDispose( uno_Environment *pEnvRemote );
};

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
        ::osl::MutexGuard guard( pImpl->m_disposingMutex );

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
          if( ppInterfaces[i]->acquire == bridges_remote::Uno2RemoteStub::thisAcquire )
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

void RemoteEnvironment::thisDispose( uno_Environment *pEnvRemote )
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
    if( ! g_bStaticDestructorsCalled  && ! pContext->m_pBridgeImpl->m_bDisposed )
    {
        // TODO : not threadsafe
        // synchronization with dispatch methods needed !

        pImpl->m_bDisposed = sal_True;


        // close the connection
        urp_sendCloseConnection( pEnvRemote );
        
        if( osl_getThreadIdentifier(0) ==
            (oslThreadIdentifier) pImpl->m_pReader->getIdentifier() )
        {
            // This is the reader thread. Let the thread destroy itself
            // the reader thread object must also release the connection at this stage !
            pImpl->m_pReader->destroyYourself();
        }
        else
        {
            // wait for the reader thread
            // the reader thread object must also release the connection,
            // when terminating !!!!
            pImpl->m_pReader->join();
        }

        // from now on, no calls can be delivered via the bridge
        uno_threadpool_dispose( pImpl->m_hThreadPool );

          pContext->m_pConnection->close( pContext->m_pConnection );
        
        // wait for the writer thread
        pImpl->m_pWriter->join();
        
        // now let the context go !
        pContext->dispose( pContext );

          if( 0 != pImpl->m_nRemoteThreads )
          {
              // Wait for all threads
              pImpl->m_cndWaitForThreads.reset();
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
#ifdef DEBUG
        pImpl->dumpErrors( stderr );
#endif
            
        // destroy the threads
        delete pImpl->m_pWriter;
        pImpl->m_pWriter = 0;
        
        if( osl_getThreadIdentifier(0) !=
            (oslThreadIdentifier) pImpl->m_pReader->getIdentifier() )
        {
            // This is not the reader thread, so the thread object is deleted
            delete pImpl->m_pReader;
        }
        pImpl->m_pReader = 0;
        
        // delete the stubs 
        releaseStubs( pEnvRemote );

    }
}

void RemoteEnvironment::thisDisposing( uno_Environment *pEnvRemote )
{
    remote_Context *pContext = (remote_Context * )pEnvRemote->pContext;
    urp_BridgeImpl *pImpl = ((urp_BridgeImpl*) pContext->m_pBridgeImpl);

    {
        ::osl::ClearableMutexGuard guard( pImpl->m_disposingMutex );
        if( ! pImpl->m_bDisposed )
        {
            guard.clear();
            thisDispose( pEnvRemote );
        }
    }
    pImpl->m_pPropertyObject->thisRelease();
    pImpl->m_pPropertyObject = 0;

     uno_threadpool_destroy( pImpl->m_hThreadPool );

    pContext->aBase.release( (uno_Context * ) pContext );
#ifdef DEBUG
      thisCounter.release();
#endif
}

void RemoteEnvironment::thisComputeObjectIdentifier( uno_ExtEnvironment *pEnvRemote,
                                                     rtl_uString **ppOid ,
                                                     void *pInterface)
{
    OSL_ENSURE( 0, "RemoteEnvironment::thisComputeObjectIdentifier should never be called" );
}

void RemoteEnvironment::thisAcquireInterface( uno_ExtEnvironment *pEnvRemote, void *pInterface )
{
    ((remote_Interface *)pInterface)->acquire( ( remote_Interface *) pInterface );
}

void RemoteEnvironment::thisReleaseInterface( uno_ExtEnvironment *pEnvRemote, void *pInterface )
{
    ((remote_Interface *)pInterface)->release( ( remote_Interface *) pInterface );
}

} // end namespace bridges_urp
using namespace bridges_urp;


//##################################################################################################
extern "C" SAL_DLLEXPORT void SAL_CALL uno_initEnvironment(
    uno_Environment * pEnvRemote )
{
    // set C-virtual methods
    pEnvRemote->environmentDisposing = RemoteEnvironment::thisDisposing;
    pEnvRemote->pExtEnv->computeObjectIdentifier = RemoteEnvironment::thisComputeObjectIdentifier;
    pEnvRemote->pExtEnv->acquireInterface = RemoteEnvironment::thisAcquireInterface;
    pEnvRemote->pExtEnv->releaseInterface = RemoteEnvironment::thisReleaseInterface;
    pEnvRemote->dispose = RemoteEnvironment::thisDispose;

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
    
    pImpl->m_cndWaitForThreads.reset();
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
    
    // create the properties object
    // start the property-set-thread, if necessary
    if( sProtocolProperties.getLength() )
    {
        PropertySetterThread *pPropsSetterThread =
            new PropertySetterThread( pEnvRemote, pImpl , sProtocolProperties );
        pPropsSetterThread->create();
    }
#ifdef DEBUG
    thisCounter.acquire();
#endif	
}


//##################################################################################################
extern "C" SAL_DLLEXPORT void SAL_CALL uno_ext_getMapping(
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
                                     bridges_remote::RemoteMapping::remoteToUno,
                                     OUString() );
        }
        else if ( sFromName.equalsIgnoreAsciiCase( sUno ) &&
                  sToName.equalsIgnoreAsciiCase( sRemote ) )
        {
            pMapping =  new bridges_remote::RemoteMapping( pFrom ,
                                           pTo ,
                                           bridges_remote::RemoteMapping::unoToRemote,
                                           OUString() );
        }
        
        *ppMapping = (uno_Mapping * )pMapping;
        OUString dummy;
        uno_registerMapping( ppMapping ,
                             bridges_remote::RemoteMapping::thisFree,
                             pFrom ,
                             pTo ,
                             dummy.pData );
    }
}


