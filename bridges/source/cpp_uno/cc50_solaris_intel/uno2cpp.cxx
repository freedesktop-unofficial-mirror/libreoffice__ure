/*************************************************************************
 *
 *  $RCSfile: uno2cpp.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: dbo $ $Date: 2001-04-11 14:07:46 $
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

#include <alloca.h>
#include <map>
#ifndef _RTL_ALLOC_H_
#include <rtl/alloc.h>
#endif

#ifndef _UNO_DATA_H_
#include <uno/data.h>
#endif
#ifndef _BRIDGES_CPP_UNO_BRIDGE_HXX_
#include <bridges/cpp_uno/bridge.hxx>
#endif
#ifndef _BRIDGES_CPP_UNO_TYPE_MISC_HXX_
#include <bridges/cpp_uno/type_misc.hxx>
#endif

#include "cc50_solaris_intel.hxx"

using namespace std;
using namespace osl;
using namespace rtl;
using namespace com::sun::star::uno;

namespace CPPU_CURRENT_NAMESPACE
{

extern "C" {
    void callVirtualMethod(
        void * pThis,
        sal_Int32 nVtableIndex,
        void * pRegisterReturn,
        typelib_TypeClass eReturnType,
        sal_Int32 * pStackLongs,
        sal_Int32 nStackLongs
        );
}

//==================================================================================================
static inline void cpp_call( cppu_unoInterfaceProxy * pThis,
                             sal_Int32 nVtableCall,
                             typelib_TypeDescriptionReference * pReturnTypeRef,
                             sal_Int32 nParams, typelib_MethodParameter * pParams,
                             sal_Int32 nExceptions, typelib_TypeDescriptionReference ** ppExceptionRefs,
                             void * pUnoReturn, void * pUnoArgs[], uno_Any ** ppUnoExc )
{
      // max space for: [complex ret ptr], values|ptr ...
      char * pCppStack = (char *)alloca( ((nParams+3) * sizeof(sal_Int64)) );
      char * pCppStackStart = pCppStack;
    
      // return
    typelib_TypeDescription * pReturnTypeDescr = 0;
    TYPELIB_DANGER_GET( &pReturnTypeDescr, pReturnTypeRef );
    OSL_ENSURE( pReturnTypeDescr, "### expected return type description!" );
    
      void * pCppReturn = 0; // if != 0 && != pUnoReturn, needs reconversion
    
    long return_buffer[64];
    void * pReturnSpace = 0;
    
    if (pReturnTypeDescr)
    {
        if (cppu_isSimpleType( pReturnTypeDescr ))
        {
            pCppReturn = pUnoReturn; // direct way for simple types
        }
        else
        {
            // complex return via ptr
            pCppReturn = *(void **)pCppStack =
                (cppu_relatesToInterface( pReturnTypeDescr )
                 ? (pReturnTypeDescr->nSize > sizeof(return_buffer) ? (pReturnSpace = rtl_allocateMemory( pReturnTypeDescr->nSize )) : return_buffer)
                 : pUnoReturn); // direct way
            pCppStack += sizeof(void *);
        }
    }
    // push this
    *(void**)pCppStack = pThis->pCppI;
    pCppStack += sizeof( void* );
    
    const int nMaxParams = 32;
    // args
    void * args_buffer[3 * nMaxParams];
    void ** pCppArgs  = (void **)(nParams > nMaxParams ? rtl_allocateMemory( 3 * sizeof(void *) * nParams ) : args_buffer);
    // indizes of values this have to be converted (interface conversion cpp<=>uno)
    sal_Int32 * pTempIndizes = (sal_Int32 *)(pCppArgs + nParams);
    // type descriptions for reconversions
    typelib_TypeDescription ** ppTempParamTypeDescr = (typelib_TypeDescription **)(pCppArgs + (2 * nParams));

      sal_Int32 nTempIndizes   = 0;

    const int nTempBufferSize = 256;
    sal_Int32 nTempBufferPos = 0;
    long params_buffer[nTempBufferSize];
    
      for ( sal_Int32 nPos = 0; nPos < nParams; ++nPos )
      {
        const typelib_MethodParameter & rParam = pParams[nPos];
        typelib_TypeDescription * pParamTypeDescr = 0;
        TYPELIB_DANGER_GET( &pParamTypeDescr, rParam.pTypeRef );
        
        if (!rParam.bOut && cppu_isSimpleType( pParamTypeDescr ))
        {
            pCppArgs[ nPos ] = pCppStack;
            uno_copyAndConvertData( pCppArgs[nPos], pUnoArgs[nPos], pParamTypeDescr,
                                    &pThis->pBridge->aUno2Cpp );
            
            switch (pParamTypeDescr->eTypeClass)
            {
            case typelib_TypeClass_HYPER:
            case typelib_TypeClass_UNSIGNED_HYPER:
            case typelib_TypeClass_DOUBLE:
                pCppStack += sizeof(sal_Int32); // extra long
            }
            // no longer needed
            TYPELIB_DANGER_RELEASE( pParamTypeDescr );
        }
        else // ptr to complex value | ref
        {
            if (! rParam.bIn) // is pure out
            {
                // cpp out is constructed mem, uno out is not!
                if (pParamTypeDescr->nSize > (sizeof(long) * (nTempBufferSize - nTempBufferPos)))
                {
                    uno_constructData(
                        *(void **)pCppStack = pCppArgs[nPos] = rtl_allocateMemory( pParamTypeDescr->nSize ),
                        pParamTypeDescr );
                    pTempIndizes[nTempIndizes] = nPos | 0x80000000; // default constructed for cpp call
                }
                else
                {
                    uno_constructData(
                        *(void **)pCppStack = pCppArgs[nPos] = (params_buffer + nTempBufferPos),
                        pParamTypeDescr );
                    pTempIndizes[nTempIndizes] = nPos; // default constructed for cpp call
                    nTempBufferPos += (pParamTypeDescr->nSize / sizeof(long)) +1;
                }
                // will be released at reconversion
                ppTempParamTypeDescr[nTempIndizes++] = pParamTypeDescr;
            }
            // is in/inout
            else if (cppu_relatesToInterface( pParamTypeDescr ))
            {
                if (pParamTypeDescr->nSize > (sizeof(long)*(nTempBufferSize - nTempBufferPos)))
                {
                    uno_copyAndConvertData(
                        *(void **)pCppStack = pCppArgs[nPos] = rtl_allocateMemory( pParamTypeDescr->nSize ),
                        pUnoArgs[nPos], pParamTypeDescr, &pThis->pBridge->aUno2Cpp );
                    pTempIndizes[nTempIndizes] = nPos | 0x80000000; // has to be reconverted
                }
                else
                {
                    uno_copyAndConvertData(
                        *(void **)pCppStack = pCppArgs[nPos] = (params_buffer + nTempBufferPos),
                        pUnoArgs[nPos], pParamTypeDescr, &pThis->pBridge->aUno2Cpp );
                    pTempIndizes[nTempIndizes] = nPos; // has to be reconverted
                    nTempBufferPos += (pParamTypeDescr->nSize / sizeof(long)) +1;
                }
                // will be released at reconversion
                ppTempParamTypeDescr[nTempIndizes++] = pParamTypeDescr;
            }
            else // direct way
            {
                *(void **)pCppStack = pCppArgs[nPos] = pUnoArgs[nPos];
                // no longer needed
                TYPELIB_DANGER_RELEASE( pParamTypeDescr );
            }
        }
        pCppStack += sizeof(sal_Int32); // standard parameter length
      }

     try
      {
        int nStackLongs = (pCppStack - pCppStackStart)/sizeof(sal_Int32);
        if( nStackLongs & 1 )
            // stack has to be 8 byte aligned
            nStackLongs++;

        callVirtualMethod(
            pThis->pCppI,
            nVtableCall,
            pCppReturn,
            pReturnTypeDescr->eTypeClass,
            (sal_Int32 *)pCppStackStart,
            nStackLongs
            );
    
        // NO exception occured...
        *ppUnoExc = 0;
        
        // reconvert temporary params
        for ( ; nTempIndizes--; )
        {
            sal_Int32 nIndex = pTempIndizes[nTempIndizes];
            sal_Bool bAllocated = (nIndex & 0x80000000) != 0;
            nIndex &= 0x7fffffff;
            typelib_TypeDescription * pParamTypeDescr = ppTempParamTypeDescr[nTempIndizes];
            
            if (pParams[nIndex].bIn)
            {
                if (pParams[nIndex].bOut) // inout
                {
                    uno_destructData( pUnoArgs[nIndex], pParamTypeDescr, 0 ); // destroy uno value
                    uno_copyAndConvertData( pUnoArgs[nIndex], pCppArgs[nIndex], pParamTypeDescr,
                                            &pThis->pBridge->aCpp2Uno );
                }
            }
            else // pure out
            {
                uno_copyAndConvertData( pUnoArgs[nIndex], pCppArgs[nIndex], pParamTypeDescr,
                                        &pThis->pBridge->aCpp2Uno );
            }
            // destroy temp cpp param => cpp: every param was constructed
            uno_destructData( pCppArgs[nIndex], pParamTypeDescr, cpp_release );

            if (bAllocated)
                rtl_freeMemory( pCppArgs[nIndex] );
            TYPELIB_DANGER_RELEASE( pParamTypeDescr );
        }
        // return value
        if (pCppReturn && pUnoReturn != pCppReturn)
        {
            uno_copyAndConvertData( pUnoReturn, pCppReturn, pReturnTypeDescr,
                                    &pThis->pBridge->aCpp2Uno );
            uno_destructData( pCppReturn, pReturnTypeDescr, cpp_release );
        }
     }
     catch( ... )
     {
          // get exception
        cc50_solaris_intel_fillUnoException( __Crun::ex_get(),
                                             __Cimpl::ex_name(),
                                             *ppUnoExc,
                                             &pThis->pBridge->aCpp2Uno );
        // temporary params
        for ( ; nTempIndizes--; )
        {
            sal_Int32 nIndex = pTempIndizes[nTempIndizes];
            sal_Bool bAllocated = (nIndex & 0x80000000) != 0;
            nIndex &= 0x7fffffff;
            // destroy temp cpp param => cpp: every param was constructed
            uno_destructData( pCppArgs[nIndex], ppTempParamTypeDescr[nTempIndizes], cpp_release );
            if (bAllocated)
                rtl_freeMemory( pCppArgs[nIndex] );
            TYPELIB_DANGER_RELEASE( ppTempParamTypeDescr[nTempIndizes] );
        }
        // return type
        if (pReturnTypeDescr)
            TYPELIB_DANGER_RELEASE( pReturnTypeDescr );
      }
    
    if (pCppArgs != (void **)args_buffer)
        rtl_freeMemory( pCppArgs );
    if (pReturnSpace)
        rtl_freeMemory( pReturnSpace );
}

//==================================================================================================
extern "C" void SAL_CALL cppu_unoInterfaceProxy_dispatch(
    uno_Interface * pUnoI, const typelib_TypeDescription * pMemberDescr,
    void * pReturn, void * pArgs[], uno_Any ** ppException ) throw ()
{
    // is my surrogate
    cppu_unoInterfaceProxy * pThis = (cppu_unoInterfaceProxy *)pUnoI;
    typelib_InterfaceTypeDescription * pTypeDescr = pThis->pTypeDescr;
    
    switch (pMemberDescr->eTypeClass)
    {
    case typelib_TypeClass_INTERFACE_ATTRIBUTE:
    {
        // determine vtable call index
        sal_Int32 nMemberPos = ((typelib_InterfaceMemberTypeDescription *)pMemberDescr)->nPosition;
        OSL_ENSURE( nMemberPos < pTypeDescr->nAllMembers, "### member pos out of range!" );
        
        sal_Int32 nVtableCall = pTypeDescr->pMapMemberIndexToFunctionIndex[nMemberPos];
        OSL_ENSURE( nVtableCall < pTypeDescr->nMapFunctionIndexToMemberIndex, "### illegal vtable index!" );
        
        typelib_TypeDescriptionReference * pRuntimeExcRef = 0;
        
        if (pReturn)
        {
            // dependent dispatch
            cpp_call( pThis, nVtableCall,
                      ((typelib_InterfaceAttributeTypeDescription *)pMemberDescr)->pAttributeTypeRef,
                      0, 0, // no params
                      1, &pRuntimeExcRef, // RuntimeException
                      pReturn, pArgs, ppException );
        }
        else
        {
            // is SET
            typelib_MethodParameter aParam;
            aParam.pTypeRef =
                ((typelib_InterfaceAttributeTypeDescription *)pMemberDescr)->pAttributeTypeRef;
            aParam.bIn		= sal_True;
            aParam.bOut		= sal_False;

            typelib_TypeDescriptionReference * pReturnTypeRef = 0;
            OUString aVoidName( RTL_CONSTASCII_USTRINGPARAM("void") );
            typelib_typedescriptionreference_new(
                &pReturnTypeRef, typelib_TypeClass_VOID, aVoidName.pData );
            
            // dependent dispatch
            cpp_call( pThis, nVtableCall +1, // get, then set method
                      pReturnTypeRef,
                      1, &aParam,
                      1, &pRuntimeExcRef,
                      pReturn, pArgs, ppException );
            
            typelib_typedescriptionreference_release( pReturnTypeRef );
        }
        
        break;
    }
    case typelib_TypeClass_INTERFACE_METHOD:
    {
        // determine vtable call index
        sal_Int32 nMemberPos = ((typelib_InterfaceMemberTypeDescription *)pMemberDescr)->nPosition;
        OSL_ENSURE( nMemberPos < pTypeDescr->nAllMembers, "### member pos out of range!" );
        
        sal_Int32 nVtableCall = pTypeDescr->pMapMemberIndexToFunctionIndex[nMemberPos];
        OSL_ENSURE( nVtableCall < pTypeDescr->nMapFunctionIndexToMemberIndex, "### illegal vtable index!" );
        
        switch (nVtableCall)
        {
            // standard calls
        case 1: // acquire uno interface
            (*pUnoI->acquire)( pUnoI );
            *ppException = 0;
            break;
        case 2: // release uno interface
            (*pUnoI->release)( pUnoI );
            *ppException = 0;
            break;
        case 0: // queryInterface() opt
        {
            typelib_TypeDescription * pTD = 0;
            TYPELIB_DANGER_GET( &pTD, reinterpret_cast< Type * >( pArgs[0] )->getTypeLibType() );
            OSL_ASSERT( pTD );
            
            uno_Interface * pInterface = 0;
            (*pThis->pBridge->pUnoEnv->getRegisteredInterface)(
                pThis->pBridge->pUnoEnv,
                (void **)&pInterface, pThis->oid.pData, (typelib_InterfaceTypeDescription *)pTD );
            
            if (pInterface)
            {
                uno_any_construct( reinterpret_cast< uno_Any * >( pReturn ), &pInterface, pTD, 0 );
                (*pInterface->release)( pInterface );
                TYPELIB_DANGER_RELEASE( pTD );
                *ppException = 0;
                break;
            }
            TYPELIB_DANGER_RELEASE( pTD );
        } // else perform queryInterface()
        default:
            // dependent dispatch
            cpp_call( pThis, nVtableCall,
                      ((typelib_InterfaceMethodTypeDescription *)pMemberDescr)->pReturnTypeRef,
                      ((typelib_InterfaceMethodTypeDescription *)pMemberDescr)->nParams,
                      ((typelib_InterfaceMethodTypeDescription *)pMemberDescr)->pParams,
                      ((typelib_InterfaceMethodTypeDescription *)pMemberDescr)->nExceptions,
                      ((typelib_InterfaceMethodTypeDescription *)pMemberDescr)->ppExceptions,
                      pReturn, pArgs, ppException );
        }
        break;
    }
    default:
    {
        ::com::sun::star::uno::RuntimeException aExc(
            OUString( RTL_CONSTASCII_USTRINGPARAM("illegal member type description!") ),
            ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >() );
        
        typelib_TypeDescription * pTD = 0;
        Type const & rExcType = ::getCppuType( &aExc );
        TYPELIB_DANGER_GET( &pTD, rExcType.getTypeLibType() );
        // binary identical null reference
        ::uno_any_construct( *ppException, &aExc, pTD, 0 );
        TYPELIB_DANGER_RELEASE( pTD );
    }
    }
}

}

