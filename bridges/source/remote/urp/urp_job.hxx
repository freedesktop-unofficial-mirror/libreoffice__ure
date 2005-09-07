/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: urp_job.hxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-07 22:46:36 $
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
#ifndef _URP_JOB_HXX_
#define _URP_JOB_HXX_
#include <list>
#include <typelib/typedescription.hxx>
#include <uno/any2.h>
#include <uno/environment.h>
#include <uno/threadpool.h>
#include "urp_threadid.hxx"
#include "urp_unmarshal.hxx"
#include "urp_bridgeimpl.hxx"


namespace bridges_urp
{
const sal_Int32 MULTIJOB_STANDARD_MEMORY_SIZE = 1024;
const sal_Int32 MULTIJOB_PER_CALL_MEMORY_SIZE = 96;

class Unmarshal;
struct urp_BridgeImpl;

template < class t >	
inline t mymax( const t &t1 , const t &t2 )
{
    return t1 > t2 ? t1 : t2;
}
    
class Job
{
public:
      Job( uno_Environment *pEnvRemote,
           sal_Sequence *pTid,
           struct urp_BridgeImpl *pBridgeImpl,
           Unmarshal *pUnmarshal );

    Job( uno_Environment *pEnvRemote,
           struct urp_BridgeImpl *pBridgeImpl,
         ::bridges_remote::RemoteThreadCounter_HoldEnvWeak value )
        : m_pBridgeImpl( pBridgeImpl )
        , m_pTid( 0 )
        , m_counter( pEnvRemote , value )
        {}

    ~Job();

       inline void setUnmarshal( Unmarshal *p )
           { m_pUnmarshal = p; }

public:
      Unmarshal *m_pUnmarshal;
    struct urp_BridgeImpl *m_pBridgeImpl; 
    sal_Sequence          *m_pTid;
    ::bridges_remote::RemoteThreadCounter m_counter;
};

class ClientJob : public Job
{
public:
    inline ClientJob( uno_Environment *pEnvRemote, // weak !
                      struct urp_BridgeImpl *pBridgeImpl,
                      rtl_uString *pOid,  // weak 
                      typelib_TypeDescription * pMemberType, // weak 
                      typelib_InterfaceTypeDescription *pInterfaceType, // weak
                      void *pReturn,
                      void *ppArgs[],
                      uno_Any **ppException	 );

    // ~ClientJob
    // no release for method type and attribute type necessary, because
    // it was acquired by the caller of urp_sendRequest. The lifetime
    // of the ClientJob object is always shorter than the urp_sendRequest call.		
    inline ~ClientJob()
        {
            if( m_bReleaseForTypeDescriptionNecessary )
                typelib_typedescription_release( (typelib_TypeDescription*) m_pInterfaceType );
            uno_releaseIdFromCurrentThread();
        }

    sal_Bool pack();
    void wait();
    sal_Bool extract( );
    void initiate();

    inline void setBridgePropertyCall()
        { m_bBridgePropertyCall = sal_True; }
    inline sal_Bool isBridgePropertyCall()
        { return m_bBridgePropertyCall; }
    inline sal_Bool isOneway()
        { return m_bOneway; }
public:
    typelib_InterfaceMethodTypeDescription    *m_pMethodType;
    typelib_InterfaceAttributeTypeDescription *m_pAttributeType;
    sal_Bool m_bExceptionOccured;

private:
    void     **m_ppArgs;
    void     *m_pReturn;
    typelib_InterfaceTypeDescription          *m_pInterfaceType;
    sal_Bool m_bReleaseForTypeDescriptionNecessary;
    
    uno_Any  **m_ppException;
    sal_Bool m_bOneway;
    sal_Bool m_bBridgePropertyCall;
    sal_uInt16 m_nMethodIndex;
    uno_Environment *m_pEnvRemote;
    rtl_uString *m_pOid;
    sal_Bool m_bCallingConventionForced;
};

struct MemberTypeInfo
{
    typelib_InterfaceTypeDescription          *m_pInterfaceType;
    typelib_InterfaceMethodTypeDescription    *m_pMethodType;
    typelib_InterfaceAttributeTypeDescription *m_pAttributeType;
    sal_Int32 m_nArgCount;
    sal_Bool  m_bIsReleaseCall;
    sal_Bool  *m_pbIsIn;
    sal_Bool  *m_pbIsOut;
    sal_Bool  m_bIsOneway;
    typelib_TypeDescription *m_pReturnType;
    typelib_TypeDescription **m_ppArgType;
};

    
struct ServerJobEntry
{
    rtl_uString           *m_pOid;
    remote_Interface      *m_pRemoteI;
    typelib_TypeDescriptionReference          *m_pInterfaceTypeRef;
    void                  **m_ppArgs;
    void                  *m_pReturn;
    uno_Any               m_exception;
     uno_Any               *m_pException;
    sal_Bool              m_bIgnoreCache;
};

class ServerMultiJob : public Job
{
public:
    ServerMultiJob( uno_Environment *pEnvRemote,
                    sal_Sequence *pTid,
                    struct urp_BridgeImpl *pBridgeImpl,
                    Unmarshal *pUnmarshal,
                    sal_Int32 nMaxMessages );
    ~ServerMultiJob();
public:
    // doit method is used only for ServerJobs, calls execute and pack
    static void  SAL_CALL doit( void *pThreadSpecificData );

    sal_Bool extract( );
    void initiate();
    void execute();

public:
    // setMethodType or setAttributeType MUST be called before extract
    inline void setMethodType(
        typelib_InterfaceMethodTypeDescription *pMethodType,
        sal_Bool bIsReleaseCall,
        sal_Bool bIsOneway )
        {
            m_aTypeInfo[m_nCalls].m_pMethodType = pMethodType;
            m_aTypeInfo[m_nCalls].m_pAttributeType = 0;
            m_aTypeInfo[m_nCalls].m_nArgCount = pMethodType->nParams;
            m_aTypeInfo[m_nCalls].m_bIsReleaseCall = bIsReleaseCall;
            m_aTypeInfo[m_nCalls].m_bIsOneway = bIsOneway;
        }
    
    inline void setAttributeType(
        typelib_InterfaceAttributeTypeDescription *pAttributeType, sal_Bool bIsSetter, sal_Bool bIsOneway )
        {
            m_aTypeInfo[m_nCalls].m_pAttributeType = pAttributeType;
            m_aTypeInfo[m_nCalls].m_pMethodType = 0;
            m_aTypeInfo[m_nCalls].m_nArgCount = bIsSetter ? 1 : 0;
            m_aTypeInfo[m_nCalls].m_bIsReleaseCall = sal_False;
            m_aTypeInfo[m_nCalls].m_bIsOneway = bIsOneway;
        }

    inline void setType( typelib_TypeDescriptionReference *pTypeRef )
        {
            m_aEntries[m_nCalls].m_pInterfaceTypeRef = pTypeRef;
            typelib_typedescriptionreference_acquire( m_aEntries[m_nCalls].m_pInterfaceTypeRef );
            TYPELIB_DANGER_GET(
                (typelib_TypeDescription ** )&(m_aTypeInfo[m_nCalls].m_pInterfaceType) ,
                pTypeRef );
        }
    // setOid or setInterface MUST be called before extract
      inline void setOid(	rtl_uString *pOid )
        {
            m_aEntries[m_nCalls].m_pOid = pOid;
            rtl_uString_acquire( m_aEntries[m_nCalls].m_pOid );
            m_aEntries[m_nCalls].m_pRemoteI = 0;
        }
        
    // setOid or setInterface MUST be called
    inline void setInterface( remote_Interface *pRemoteI )
        {
            m_aEntries[m_nCalls].m_pRemoteI = pRemoteI;
            pRemoteI->acquire( pRemoteI );
            m_aEntries[m_nCalls].m_pOid = 0;
        }

    inline void setIgnoreCache( sal_Bool bIgnoreCache )
        {
            m_aEntries[m_nCalls].m_bIgnoreCache = bIgnoreCache;
        }

    inline sal_Bool isFull()
        { return m_nCalls >= m_nMaxMessages; }

    inline sal_Int8 *getHeap( sal_Int32 nSizeToAlloc )
        {
            if( nSizeToAlloc + m_nCurrentMemPosition > m_nCurrentMemSize )
            {
                m_lstMem.push_back( m_pCurrentMem );
                m_nCurrentMemSize = mymax( nSizeToAlloc , MULTIJOB_STANDARD_MEMORY_SIZE ) +
                    (m_nMaxMessages -m_nCalls)*MULTIJOB_PER_CALL_MEMORY_SIZE;
                m_pCurrentMem = (sal_Int8*)	rtl_allocateMemory( m_nCurrentMemSize );
                m_nCurrentMemPosition = 0;
            }
            sal_Int8 *pHeap = m_pCurrentMem + m_nCurrentMemPosition;
            m_nCurrentMemPosition += nSizeToAlloc;

            // care for alignment
            if( m_nCurrentMemPosition & 0x7 )
            {
                m_nCurrentMemPosition = ( ((sal_uInt32)m_nCurrentMemPosition) & ( 0xffffffff - 0x7 )) + 8;
            }
            return pHeap;
        }
      void prepareRuntimeException( const ::rtl::OUString &sMessage, sal_Int32 nCall );

private:
    uno_Environment *m_pEnvRemote;
    sal_Int32 m_nCalls;
    sal_Int32 m_nMaxMessages;
    
    ServerJobEntry *m_aEntries;
    MemberTypeInfo *m_aTypeInfo;

    sal_Int8 *m_pCurrentMem;
    sal_Int32 m_nCurrentMemSize;
    sal_Int32 m_nCurrentMemPosition;

    // list of memory pointers, that must be freed
    ::std::list< sal_Int8 * > m_lstMem;
};



//---------------------------------------------------------------------------------------------
inline ClientJob::ClientJob(
    uno_Environment *pEnvRemote,
    struct urp_BridgeImpl *pBridgeImpl,
    rtl_uString *pOid,
    typelib_TypeDescription * pMemberType,
    typelib_InterfaceTypeDescription *pInterfaceType,
    void *pReturn,
    void *ppArgs[],
    uno_Any **ppException )
    : Job( pEnvRemote , pBridgeImpl, ::bridges_remote::RTC_HOLDENVWEAK )
    , m_ppArgs( ppArgs )
    , m_pReturn( pReturn )
    , m_pInterfaceType( pInterfaceType ) // weak 
    , m_bReleaseForTypeDescriptionNecessary( sal_False )
    , m_ppException( ppException )
    , m_bBridgePropertyCall( sal_False )
    , m_pEnvRemote( pEnvRemote ) // weak
    , m_pOid( pOid ) // weak
    , m_bCallingConventionForced( sal_False )
{
    uno_getIdOfCurrentThread( &m_pTid );

    if( typelib_TypeClass_INTERFACE_METHOD == pMemberType->eTypeClass )
    {
        m_pMethodType = ( typelib_InterfaceMethodTypeDescription * ) pMemberType;
        m_pAttributeType = 0;
    }
    else if( typelib_TypeClass_INTERFACE_ATTRIBUTE == pMemberType->eTypeClass )
    {
        m_pAttributeType = ( typelib_InterfaceAttributeTypeDescription * ) pMemberType;
        m_pMethodType = 0;
    }
    else
    {
        OSL_ASSERT( ! "wrong member type" );
    }

    // calculate method index
    if( ! m_pInterfaceType->aBase.bComplete )
    {
        // must be acquired because typedescription may be exchanged
        typelib_typedescription_acquire((typelib_TypeDescription*) m_pInterfaceType );
        m_bReleaseForTypeDescriptionNecessary = sal_True;
        typelib_typedescription_complete( (typelib_TypeDescription ** ) &m_pInterfaceType );
    }
    m_nMethodIndex = (sal_uInt16) m_pInterfaceType->pMapMemberIndexToFunctionIndex[
        ((typelib_InterfaceMemberTypeDescription*)pMemberType)->nPosition ];
    
    if( m_pAttributeType && m_ppArgs )
    {
        // setter
        m_nMethodIndex ++;
    }

    if( typelib_TypeClass_INTERFACE_METHOD == pMemberType->eTypeClass )
    {
//  		if( (( typelib_InterfaceMemberTypeDescription * ) pMemberType)->nPosition
//  			== REMOTE_RELEASE_METHOD_INDEX )
//  		{
//  			m_bOneway = sal_True;
//  		}
//  		else
        if( pBridgeImpl->m_properties.bForceSynchronous )
        {
            m_bOneway = sal_False;
            if( (( typelib_InterfaceMethodTypeDescription * ) pMemberType)->bOneWay )
            {
                m_bCallingConventionForced = sal_True;
            }
        }
        else
        {
            m_bOneway = (( typelib_InterfaceMethodTypeDescription * ) pMemberType)->bOneWay;
        }
    }
    else
    {
        m_bOneway = sal_False;
    }
}

}
#endif
