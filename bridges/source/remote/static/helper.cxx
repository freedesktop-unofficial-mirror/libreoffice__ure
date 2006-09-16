/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: helper.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-16 15:59:10 $
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
#include <rtl/alloc.h>
#include <osl/diagnose.h>

#include <bridges/remote/helper.hxx>

#include <bridges/remote/stub.hxx>
#include <bridges/remote/proxy.hxx>
#include <bridges/remote/remote.hxx>

#include <com/sun/star/uno/Sequence.hxx>

using namespace ::rtl;
using namespace ::com::sun::star::uno;

namespace bridges_remote
{
    
void SAL_CALL remote_createStub (
    remote_Interface **ppRemoteI,
    rtl_uString *pOid ,
    typelib_TypeDescriptionReference *pTypeRef,
    uno_Environment *pEnvRemote,
    ReleaseRemoteCallbackFunc releaseRemoteCallback )
{
    typelib_TypeDescription *pType = 0;
    typelib_typedescriptionreference_getDescription( &pType, pTypeRef );	

    (void) pEnvRemote->pExtEnv->getRegisteredInterface(
        pEnvRemote->pExtEnv,
        (void **)ppRemoteI,
        pOid,
        (typelib_InterfaceTypeDescription* )pType );

    if( *ppRemoteI )
    {
        if( (*ppRemoteI)->acquire == acquireRemote2RemoteStub ) {

              if( releaseRemoteCallback )
              {
                  // use the callback handler, the bridge wants to send the call immeadiatly
                  releaseRemoteCallback( *ppRemoteI , pOid, pTypeRef , pEnvRemote );
              }
              else
              {
                ((::bridges_remote::Remote2RemoteStub *)*ppRemoteI)->releaseRemote();
              }
        }
        else
        {
            // Uno2RemoteStub
            // no release necessary
        }
    }
    else
    {
        remote_BridgeImpl *pImpl = ((remote_Context *)pEnvRemote->pContext)->m_pBridgeImpl;
        *ppRemoteI =
            new ::bridges_remote::Remote2RemoteStub(
                pOid,
                (typelib_InterfaceTypeDescription * ) pType,
                pEnvRemote,
                pImpl->m_sendRequest);
        
        // ppRemoteI may change during registration
        pEnvRemote->pExtEnv->registerProxyInterface(
            pEnvRemote->pExtEnv,
            (void **) ppRemoteI,
            freeRemote2RemoteStub,
            pOid,
            (typelib_InterfaceTypeDescription * ) pType );
    }

    typelib_typedescription_release( pType );
}

void SAL_CALL remote_sendQueryInterface(
    uno_Environment *pEnvRemote,
    remote_Interface **ppRemoteI,
    rtl_uString *pOid ,
    typelib_TypeDescriptionReference *pTypeRef,
    uno_Any **ppException
    )
{
    OSL_ASSERT( ppRemoteI );

    typelib_InterfaceTypeDescription *pType = 0;
    typelib_typedescriptionreference_getDescription( (typelib_TypeDescription ** )&pType, pTypeRef );
    
    if( *ppRemoteI )
    {
        (*ppRemoteI)->release( *ppRemoteI );
        (*ppRemoteI) = 0;
    }
    
    remote_BridgeImpl *pImpl = ((remote_Context *)pEnvRemote->pContext)->m_pBridgeImpl;
    
    Type type  = ::getCppuType( (Reference < XInterface > *)0 );
    
    // get type for queryInterface
    OUString sCompleteMethodName = type.getTypeName();
    sCompleteMethodName += OUString::createFromAscii("::queryInterface");

    typelib_InterfaceMemberTypeDescription *pMemberType = 0;
    typelib_typedescription_getByName(
        (typelib_TypeDescription **) &pMemberType,
        sCompleteMethodName.pData );
    
    OSL_ASSERT( pMemberType );
    
    uno_Any anyInterface;
    anyInterface.pType = 0;
    anyInterface.pData = 0;
    
    void *pReturn = &anyInterface;
    void *ppArgs[1];

    ppArgs[0] = 0;
    typelib_TypeDescriptionReference *pRef = 0;
    typelib_typedescriptionreference_new( &pRef ,
                                          pType->aBase.eTypeClass,
                                          pType->aBase.pTypeName);

    ppArgs[0] = &pRef;
    
//  	uno_Any anyException;
//  	uno_Any *pAnyException = &anyException;

    // do the queryInterface
    pImpl->m_sendRequest(
        pEnvRemote,
        (typelib_TypeDescription * ) pMemberType,
        pOid,
        pType,
        pReturn,
        ppArgs,
        ppException );

    
    // now release everything
    typelib_typedescriptionreference_release( pRef );
    typelib_typedescription_release( (typelib_TypeDescription * ) pMemberType );
    
    if( *ppException )
    {
        *ppRemoteI = 0;
    }
    else
    {
        // set out parameter
        if( typelib_TypeClass_INTERFACE == anyInterface.pType->eTypeClass )
        {
            *ppRemoteI = ( remote_Interface * )  anyInterface.pReserved;
        }
        typelib_typedescriptionreference_release( anyInterface.pType );
    }

    typelib_typedescription_release( (typelib_TypeDescription * ) pType );
}


void SAL_CALL remote_retrieveOidFromProxy(
    remote_Interface *pRemoteI,
    rtl_uString **ppOid )
{
    if( pRemoteI->acquire == acquireRemote2RemoteStub )
    {
        // Remote2RemoteStub
        ::bridges_remote::Remote2RemoteStub *pStub = (::bridges_remote::Remote2RemoteStub * ) pRemoteI;
        rtl_uString_newFromString( ppOid , pStub->m_sOid.pData );
    }
    else
    {
        // Uno2RemoteStub
        ::bridges_remote::Uno2RemoteStub *pStub = (::bridges_remote::Uno2RemoteStub * ) pRemoteI;
        rtl_uString_newFromString( ppOid , pStub->m_sOid.pData );
        pRemoteI->acquire( pRemoteI );
    }
}

}
