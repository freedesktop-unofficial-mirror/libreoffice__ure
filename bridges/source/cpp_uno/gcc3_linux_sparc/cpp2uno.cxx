/*************************************************************************
 *
 *  $RCSfile: cpp2uno.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: hr $ $Date: 2004-02-04 13:41:57 $
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

#define LEAK_STATIC_DATA
//  #define TRACE(x) OSL_TRACE(x)
#define TRACE(x)

#include <malloc.h>
#include <list>
#include <map>
#include <typeinfo>
#ifndef _RTL_ALLOC_H_
#include <rtl/alloc.h>
#endif
#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif

#ifndef _TYPELIB_TYPEDESCRIPTION_HXX_
#include <typelib/typedescription.hxx>
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

#include "share.hxx"

using namespace com::sun::star::uno;
using namespace std;
using namespace osl;
using namespace rtl;

namespace CPPU_CURRENT_NAMESPACE
{

//==================================================================================================
rtl_StandardModuleCount g_moduleCount = MODULE_COUNT_INIT;

//==================================================================================================
static typelib_TypeClass cpp2uno_call(
    cppu_cppInterfaceProxy * pThis,
    const typelib_TypeDescription * pMemberTypeDescr,
    typelib_TypeDescriptionReference * pReturnTypeRef, // 0 indicates void return
    sal_Int32 nParams, typelib_MethodParameter * pParams,
    void ** pCallStack,
    sal_Int64 * pRegisterReturn /* space for register return */ )
{
    // pCallStack: [ret ptr], this, params
    char * pCppStack = (char *)pCallStack;

    // return
    typelib_TypeDescription * pReturnTypeDescr = 0;
    if (pReturnTypeRef)
        TYPELIB_DANGER_GET( &pReturnTypeDescr, pReturnTypeRef );
    
    void * pUnoReturn = 0;
    void * pCppReturn = 0; // complex return ptr: if != 0 && != pUnoReturn, reconversion need
    
    if (pReturnTypeDescr)
    {
        if (cppu_isSimpleType( pReturnTypeDescr ))
            pUnoReturn = pRegisterReturn; // direct way for simple types
        else // complex return via ptr (pCppReturn)
        {
            pCppReturn = *(void**)pCppStack;
            
            pUnoReturn = (cppu_relatesToInterface( pReturnTypeDescr )
                          ? alloca( pReturnTypeDescr->nSize )
                          : pCppReturn); // direct way
            pCppStack += sizeof( void* );
        }
    }
    // pop this
    pCppStack += sizeof( void* );

    // stack space
    OSL_ENSURE( sizeof(void *) == sizeof(sal_Int32), "### unexpected size!" );
    // parameters
    void ** pUnoArgs = (void **)alloca( 4 * sizeof(void *) * nParams );
    void ** pCppArgs = pUnoArgs + nParams;
    // indizes of values this have to be converted (interface conversion cpp<=>uno)
    sal_Int32 * pTempIndizes = (sal_Int32 *)(pUnoArgs + (2 * nParams));
    // type descriptions for reconversions
    typelib_TypeDescription ** ppTempParamTypeDescr = (typelib_TypeDescription **)(pUnoArgs + (3 * nParams));
    
    sal_Int32 nTempIndizes   = 0;
    
    for ( sal_Int32 nPos = 0; nPos < nParams; ++nPos )
    {
        const typelib_MethodParameter & rParam = pParams[nPos];
        typelib_TypeDescription * pParamTypeDescr = 0;
        TYPELIB_DANGER_GET( &pParamTypeDescr, rParam.pTypeRef );

        if (!rParam.bOut && cppu_isSimpleType( pParamTypeDescr )) // value
        {
            pCppArgs[nPos] = pUnoArgs[nPos] =
                adjustPointer( pCppStack, pParamTypeDescr );
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
            pCppArgs[nPos] = *(void **)pCppStack;

            if (! rParam.bIn) // is pure out
            {
                // uno out is unconstructed mem!
                pUnoArgs[nPos] = alloca( pParamTypeDescr->nSize );
                pTempIndizes[nTempIndizes] = nPos;
                // will be released at reconversion
                ppTempParamTypeDescr[nTempIndizes++] = pParamTypeDescr;
            }
            // is in/inout
            else if (cppu_relatesToInterface( pParamTypeDescr ))
            {
                uno_copyAndConvertData( pUnoArgs[nPos] = alloca( pParamTypeDescr->nSize ),
                                        *(void **)pCppStack, pParamTypeDescr,
                                        &pThis->pBridge->aCpp2Uno );
                pTempIndizes[nTempIndizes] = nPos; // has to be reconverted
                // will be released at reconversion
                ppTempParamTypeDescr[nTempIndizes++] = pParamTypeDescr;
            }
            else // direct way
            {
                pUnoArgs[nPos] = *(void **)pCppStack;
                // no longer needed
                TYPELIB_DANGER_RELEASE( pParamTypeDescr );
            }
        }
        pCppStack += sizeof(sal_Int32); // standard parameter length
    }
    
    // ExceptionHolder
    uno_Any aUnoExc; // Any will be constructed by callee
    uno_Any * pUnoExc = &aUnoExc;

    // invoke uno dispatch call
    (*pThis->pUnoI->pDispatcher)( pThis->pUnoI, pMemberTypeDescr, pUnoReturn, pUnoArgs, &pUnoExc );
    
    // in case an exception occured...
    if (pUnoExc)
    {
        // destruct temporary in/inout params
        for ( ; nTempIndizes--; )
        {
            sal_Int32 nIndex = pTempIndizes[nTempIndizes];
            
            if (pParams[nIndex].bIn) // is in/inout => was constructed
                uno_destructData( pUnoArgs[nIndex], ppTempParamTypeDescr[nTempIndizes], 0 );
            TYPELIB_DANGER_RELEASE( ppTempParamTypeDescr[nTempIndizes] );
        }
        if (pReturnTypeDescr)
            TYPELIB_DANGER_RELEASE( pReturnTypeDescr );
        
        raiseException( &aUnoExc, &pThis->pBridge->aUno2Cpp ); // has to destruct the any
        // is here for dummy
        return typelib_TypeClass_VOID;
    }
    else // else no exception occured...
    {
        // temporary params
        for ( ; nTempIndizes--; )
        {
            sal_Int32 nIndex = pTempIndizes[nTempIndizes];
            typelib_TypeDescription * pParamTypeDescr = ppTempParamTypeDescr[nTempIndizes];
            
            if (pParams[nIndex].bOut) // inout/out
            {
                // convert and assign
                uno_destructData( pCppArgs[nIndex], pParamTypeDescr, cpp_release );
                uno_copyAndConvertData( pCppArgs[nIndex], pUnoArgs[nIndex], pParamTypeDescr,
                                        &pThis->pBridge->aUno2Cpp );
            }
            // destroy temp uno param
            uno_destructData( pUnoArgs[nIndex], pParamTypeDescr, 0 );
            
            TYPELIB_DANGER_RELEASE( pParamTypeDescr );
        }
        // return
        if (pCppReturn) // has complex return
        {
            if (pUnoReturn != pCppReturn) // needs reconversion
            {
                uno_copyAndConvertData( pCppReturn, pUnoReturn, pReturnTypeDescr,
                                        &pThis->pBridge->aUno2Cpp );
                // destroy temp uno return
                uno_destructData( pUnoReturn, pReturnTypeDescr, 0 );
            }
            // complex return ptr is set to eax
            *(void **)pRegisterReturn = pCppReturn;
        }
        if (pReturnTypeDescr)
        {
            typelib_TypeClass eRet = (typelib_TypeClass)pReturnTypeDescr->eTypeClass;
            TYPELIB_DANGER_RELEASE( pReturnTypeDescr );
            return eRet;
        }
        else
            return typelib_TypeClass_VOID;
    }
}


//==================================================================================================
static typelib_TypeClass cpp_mediate(
    sal_Int32 nVtableCall,
    void ** pCallStack,
    sal_Int64 * pRegisterReturn /* space for register return */ )
{
    OSL_ENSURE( sizeof(sal_Int32)==sizeof(void *), "### unexpected!" );
    
    // pCallStack: this, params
    // eventual [ret*] lies at pCallStack -1
    // so count down pCallStack by one to keep it simple
    // _this_ ptr is patched cppu_XInterfaceProxy object
    cppu_cppInterfaceProxy * pCppI = NULL;
    pCppI = (cppu_cppInterfaceProxy *)(XInterface *)*pCallStack;
    if( nVtableCall & 0x80000000 )
    {
        pCallStack--;
        nVtableCall &= 0x7fffffff;
    }

    typelib_InterfaceTypeDescription * pTypeDescr = pCppI->pTypeDescr;
    
    OSL_ENSURE( nVtableCall < pTypeDescr->nMapFunctionIndexToMemberIndex,
                 "### illegal vtable index!" );
    if (nVtableCall >= pTypeDescr->nMapFunctionIndexToMemberIndex)
    {
        throw RuntimeException( OUString::createFromAscii("illegal vtable index!"), (XInterface *)pCppI );
    }
    
    // determine called method
    OSL_ENSURE( nVtableCall < pTypeDescr->nMapFunctionIndexToMemberIndex, "### illegal vtable index!" );
    sal_Int32 nMemberPos = pTypeDescr->pMapFunctionIndexToMemberIndex[nVtableCall];
    OSL_ENSURE( nMemberPos < pTypeDescr->nAllMembers, "### illegal member index!" );

    TypeDescription aMemberDescr( pTypeDescr->ppAllMembers[nMemberPos] );

#if defined BRIDGES_DEBUG
    OString cstr( OUStringToOString( aMemberDescr.get()->pTypeName, RTL_TEXTENCODING_ASCII_US ) );
    fprintf( stderr, "calling %s, nVtableCall=%d\n", cstr.getStr(), nVtableCall );
#endif

    typelib_TypeClass eRet;
    switch (aMemberDescr.get()->eTypeClass)
    {
    case typelib_TypeClass_INTERFACE_ATTRIBUTE:
    {
        if (pTypeDescr->pMapMemberIndexToFunctionIndex[nMemberPos] == nVtableCall)
        {
            // is GET method
            eRet = cpp2uno_call(
                pCppI, aMemberDescr.get(),
                ((typelib_InterfaceAttributeTypeDescription *)aMemberDescr.get())->pAttributeTypeRef,
                0, 0, // no params
                pCallStack, pRegisterReturn );
        }
        else
        {
            // is SET method
            typelib_MethodParameter aParam;
            aParam.pTypeRef =
                ((typelib_InterfaceAttributeTypeDescription *)aMemberDescr.get())->pAttributeTypeRef;
            aParam.bIn		= sal_True;
            aParam.bOut		= sal_False;
            
            eRet = cpp2uno_call(
                pCppI, aMemberDescr.get(),
                0, // indicates void return
                1, &aParam,
                pCallStack, pRegisterReturn );
        }
        break;
    }
    case typelib_TypeClass_INTERFACE_METHOD:
    {
        // is METHOD
        switch (nVtableCall)
        {
        case 1: // acquire()
            pCppI->acquireProxy(); // non virtual call!
            eRet = typelib_TypeClass_VOID;
            break;
        case 2: // release()
            pCppI->releaseProxy(); // non virtual call!
            eRet = typelib_TypeClass_VOID;
            break;
        case 0: // queryInterface() opt
        {
            typelib_TypeDescription * pTD = 0;
            TYPELIB_DANGER_GET( &pTD, reinterpret_cast< Type * >( pCallStack[2] )->getTypeLibType() );
            if (pTD)
            {
                XInterface * pInterface = 0;
                (*pCppI->pBridge->pCppEnv->getRegisteredInterface)(
                    pCppI->pBridge->pCppEnv,
                    (void **)&pInterface, pCppI->oid.pData, (typelib_InterfaceTypeDescription *)pTD );
            
                if (pInterface)
                {
                    ::uno_any_construct(
                        reinterpret_cast< uno_Any * >( pCallStack[0] ),
                        &pInterface, pTD, cpp_acquire );
                    pInterface->release();
                    TYPELIB_DANGER_RELEASE( pTD );
                    *(void **)pRegisterReturn = pCallStack[0];
                    eRet = typelib_TypeClass_ANY;
                    break;
                }
                TYPELIB_DANGER_RELEASE( pTD );
            }
        } // else perform queryInterface()
        default:
            eRet = cpp2uno_call(
                pCppI, aMemberDescr.get(),
                ((typelib_InterfaceMethodTypeDescription *)aMemberDescr.get())->pReturnTypeRef,
                ((typelib_InterfaceMethodTypeDescription *)aMemberDescr.get())->nParams,
                ((typelib_InterfaceMethodTypeDescription *)aMemberDescr.get())->pParams,
                pCallStack, pRegisterReturn );
        }
        break;
    }
    default:
    {
        throw RuntimeException( OUString::createFromAscii("no member description found!"), (XInterface *)pCppI );
        // is here for dummy
        eRet = typelib_TypeClass_VOID;
    }
    }
    
    return eRet;
}

//==================================================================================================
class MediateClassData
{
public:
    struct ClassDataBuffer
    {
        void*			m_pVTable;

        ~ClassDataBuffer();
    };
private:

    map< OUString, ClassDataBuffer* >		m_aClassData;
    Mutex									m_aMutex;

    void createVTable( ClassDataBuffer*, typelib_InterfaceTypeDescription* );
public:
    const ClassDataBuffer* getClassData( typelib_InterfaceTypeDescription* );
    
    MediateClassData() {}
    ~MediateClassData();
};
//__________________________________________________________________________________________________
MediateClassData::ClassDataBuffer::~ClassDataBuffer()
{
    delete m_pVTable;
}

//__________________________________________________________________________________________________
MediateClassData::~MediateClassData()
{
    TRACE( "> calling ~MediateClassData(): freeing mediate vtables... <\n" );
    
    // this MUST be the absolute last one which is called!
    for ( map< OUString, ClassDataBuffer* >::iterator iPos( m_aClassData.begin() ); iPos != m_aClassData.end(); ++iPos )
    {
        // todo
//  		delete (*iPos).second;
    }
}

//__________________________________________________________________________________________________

const MediateClassData::ClassDataBuffer* MediateClassData::getClassData( typelib_InterfaceTypeDescription* pType )
{
    MutexGuard aGuard( m_aMutex );

    map< OUString, ClassDataBuffer* >::iterator element = m_aClassData.find( pType->aBase.pTypeName );
    if( element != m_aClassData.end() )
        return (*element).second;

    ClassDataBuffer* pBuffer = new ClassDataBuffer();
    createVTable( pBuffer, pType );
    m_aClassData[ pType->aBase.pTypeName ] = pBuffer;
    return pBuffer;
}


//==================================================================================================
/**
 * is called on incoming vtable calls
 * (called by asm snippets)
 */
static void cpp_vtable_call()
{
    int nTableEntry;
    void** pCallStack;
    volatile sal_Int64 nRegReturn;

    // nTableEntry and pCallStack are delivered in registers as usual
    // but cpp_vtable_call is declared void.
    // the reason is that this way the compiler won't clobber the
    // call stack prepared by the assembler snippet to save its input
    // registers
    // also restore %i2 here which was clobbered to jump here
    __asm__( "st %%i0, %0\n\t"
             "st %%i1, %1\n\t"
             "ld [%%fp+68], %%i0\n\t"
             "ld [%%fp+72], %%i1\n\t"
             "ld [%%fp+76], %%i2\n\t"
             : : "m"(nTableEntry), "m"(pCallStack) );

    sal_Bool bComplex = nTableEntry & 0x80000000 ? sal_True : sal_False;

    typelib_TypeClass aType =
        cpp_mediate( nTableEntry, pCallStack+17, (sal_Int64*)&nRegReturn );

    switch( aType )
    {
        case typelib_TypeClass_BOOLEAN:
        case typelib_TypeClass_BYTE:
            __asm__( "ld %0, %%l0\n\t"
                     "ldsb [%%l0], %%i0\n"
                     : : "m"(&nRegReturn) );
            break;
        case typelib_TypeClass_CHAR:
        case typelib_TypeClass_SHORT:
        case typelib_TypeClass_UNSIGNED_SHORT:
            __asm__( "ld %0, %%l0\n\t"
                     "ldsh [%%l0], %%i0\n"
                     : : "m"(&nRegReturn) );
            break;
        case typelib_TypeClass_HYPER:
        case typelib_TypeClass_UNSIGNED_HYPER:

            __asm__( "ld %0, %%l0\n\t"
                     "ld [%%l0], %%i0\n\t"
                     "ld %1, %%l0\n\t"
                     "ld [%%l0], %%i1\n\t"
                     : : "m"(&nRegReturn), "m"(((long*)&nRegReturn) +1) );

            break;
        case typelib_TypeClass_FLOAT:
            __asm__( "ld %0, %%l0\n\t"
                     "ld [%%l0], %%f0\n"
                     : : "m"(&nRegReturn) );
            break;
        case typelib_TypeClass_DOUBLE:
            __asm__( "ld %0, %%l0\n\t"
                     "ldd [%%l0], %%f0\n"
                     : : "m"(&nRegReturn) );
            break;
        case typelib_TypeClass_VOID:
            break;
        default:
            __asm__( "ld %0, %%l0\n\t"
                     "ld [%%l0], %%i0\n"
                     : : "m"(&nRegReturn) );
            break;
    }

    if( bComplex )
    {
        __asm__( "add %i7, 4, %i7\n\t" );
        // after call to complex return valued funcion there is an unimp instruction
    }

}
//__________________________________________________________________________________________________

void MediateClassData::createVTable( ClassDataBuffer* pBuffer, typelib_InterfaceTypeDescription* pType )
{
    // get all member functions
    list< sal_Bool > aComplexReturn;

    for( int n = 0; n < pType->nAllMembers; n++ )
    {
        typelib_TypeDescription* pMember = NULL;
        TYPELIB_DANGER_GET( &pMember, pType->ppAllMembers[n] );
        if( pMember->eTypeClass == typelib_TypeClass_INTERFACE_ATTRIBUTE )
        {
            typelib_TypeDescription * pRetTD = 0;
            TYPELIB_DANGER_GET( &pRetTD, ((typelib_InterfaceAttributeTypeDescription *)pMember)->pAttributeTypeRef );
            // get method
            aComplexReturn.push_back( !cppu_isSimpleType( pRetTD ) );
            // set method
            if( ! ((typelib_InterfaceAttributeTypeDescription*)pMember)->bReadOnly )
                aComplexReturn.push_back( sal_False );
            TYPELIB_DANGER_RELEASE( pRetTD );
        }
        else
        {
            typelib_TypeDescription * pRetTD = 0;
            TYPELIB_DANGER_GET( &pRetTD, ((typelib_InterfaceMethodTypeDescription *)pMember)->pReturnTypeRef );
            aComplexReturn.push_back( !cppu_isSimpleType( pRetTD ) );
            TYPELIB_DANGER_RELEASE( pRetTD );
        }
        TYPELIB_DANGER_RELEASE( pMember );
    }

    int nSize = aComplexReturn.size();
    const int nSnippetSize = 64;
    char * pSpace = (char *)rtl_allocateMemory( (2*(nSize+2)*sizeof(void *)) + (nSize*nSnippetSize) );
    pBuffer->m_pVTable = (void*)pSpace;
    
    char * pCode	= pSpace + (2*(nSize+2)*sizeof(void *));
    void ** pvft	= (void **)pSpace;

    // setup vft and code
    for ( sal_Int32 nPos = 0; nPos < nSize; ++nPos )
    {
        unsigned long * codeSnip = (unsigned long *)(pCode + (nPos*nSnippetSize));
        pvft[ nPos ] = codeSnip;
        unsigned long nTablePos = nPos;
        sal_Bool bComplex = aComplexReturn.front();
        if( bComplex )
            nTablePos |= 0x80000000;
        aComplexReturn.pop_front();

        /*
         *	generate this code
         * 
         *	st %o0, [%sp+68]				save registers
          *	st %o1, [%sp+72]
          *	st %o2, [%sp+76]
          *	st %o3, [%sp+80]
          *	st %o4, [%sp+84]
          *	st %o5, [%sp+88]
         *
         *	mov %sp, %o1					prepare stack ptr for cpp_vtable_call
         *	sethi %hi( nTablePos ), %o0		prepare table entry
         *	or %lo( nTablePos ), %o0		(on complex return set high bit
         *
         *	sethi $hi( cpp_vtable_call ), %l0
         *	or %l0, %lo( cpp_vtable_call ), %l0
         *	jmp %l0
         *	nop
         *
         *	Note: %o0 should be restored by cpp_vtable_call if void returned
         *	%o1 should be restored if not hyper returned
         *	%o2 must be restored
         *
         */
        *codeSnip++	= 0xd023a044;
        *codeSnip++ = 0xd223a048;
        *codeSnip++ = 0xd423a04c;
        *codeSnip++ = 0xd623a050;
        *codeSnip++ = 0xd823a054;
        *codeSnip++ = 0xda23a058;

        *codeSnip++ = 0x9210000e;
        *codeSnip++ = 0x11000000 | ( nTablePos >> 10 );
        *codeSnip++ = 0x90122000 | ( nTablePos & 1023 );
        *codeSnip++ = 0x15000000 | ( ((unsigned long)cpp_vtable_call) >> 10 );
        *codeSnip++	= 0x9412a000 | ( ((unsigned long)cpp_vtable_call) & 1023 );
        *codeSnip++ = 0x81c28000;
        *codeSnip++ = 0x01000000;		
    }
}

//==================================================================================================
void SAL_CALL cppu_cppInterfaceProxy_patchVtable(
    XInterface * pCppI, typelib_InterfaceTypeDescription * pTypeDescr ) throw ()
{
    static MediateClassData * s_pMediateClassData = 0;
    if (! s_pMediateClassData)
    {
        MutexGuard aGuard( Mutex::getGlobalMutex() );
        if (! s_pMediateClassData)
        {
#ifdef LEAK_STATIC_DATA
            s_pMediateClassData = new MediateClassData();
#else
            static MediateClassData s_aMediateClassData;
            s_pMediateClassData = &s_aMediateClassData;
#endif
        }
    }
    *(const void **)pCppI = s_pMediateClassData->getClassData( pTypeDescr )->m_pVTable;
}

}

//##################################################################################################
extern "C" SAL_DLLEXPORT sal_Bool SAL_CALL component_canUnload( TimeValue * pTime )
    SAL_THROW_EXTERN_C()
{
    return CPPU_CURRENT_NAMESPACE::g_moduleCount.canUnload( &CPPU_CURRENT_NAMESPACE::g_moduleCount, pTime );
}
//##################################################################################################
extern "C" SAL_DLLEXPORT void SAL_CALL uno_initEnvironment( uno_Environment * pCppEnv )
    SAL_THROW_EXTERN_C()
{
    CPPU_CURRENT_NAMESPACE::cppu_cppenv_initEnvironment( pCppEnv );
}
//##################################################################################################
extern "C" SAL_DLLEXPORT void SAL_CALL uno_ext_getMapping(
    uno_Mapping ** ppMapping, uno_Environment * pFrom, uno_Environment * pTo )
    SAL_THROW_EXTERN_C()
{
    CPPU_CURRENT_NAMESPACE::cppu_ext_getMapping( ppMapping, pFrom, pTo );
}

