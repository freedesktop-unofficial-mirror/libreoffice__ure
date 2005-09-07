/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: cpp2uno.cxx,v $
 *
 *  $Revision: 1.14 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-07 22:32:24 $
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

#include <malloc.h>

#include <com/sun/star/uno/genfunc.hxx>
#include <uno/data.h>
#include <typelib/typedescription.hxx>

#include "bridges/cpp_uno/shared/bridge.hxx"
#include "bridges/cpp_uno/shared/cppinterfaceproxy.hxx"
#include "bridges/cpp_uno/shared/types.hxx"
#include "bridges/cpp_uno/shared/vtablefactory.hxx"

#include "msci.hxx"

using namespace ::com::sun::star::uno;

namespace
{

//==================================================================================================
static inline typelib_TypeClass cpp2uno_call(
    bridges::cpp_uno::shared::CppInterfaceProxy * pThis,
    const typelib_TypeDescription * pMemberTypeDescr,
    typelib_TypeDescriptionReference * pReturnTypeRef, // 0 indicates void return
    sal_Int32 nParams, typelib_MethodParameter * pParams,
    void ** pCallStack,
    sal_Int64 * pRegisterReturn /* space for register return */ )
{
    // pCallStack: ret, this, [complex return ptr], params
    char * pCppStack = (char *)(pCallStack +2);

    // return
    typelib_TypeDescription * pReturnTypeDescr = 0;
    if (pReturnTypeRef)
    {
        TYPELIB_DANGER_GET( &pReturnTypeDescr, pReturnTypeRef );
    }

    void * pUnoReturn = 0;
    void * pCppReturn = 0; // complex return ptr: if != 0 && != pUnoReturn, reconversion need

    if (pReturnTypeDescr)
    {
        if (bridges::cpp_uno::shared::isSimpleType( pReturnTypeDescr ))
        {
            pUnoReturn = pRegisterReturn; // direct way for simple types
        }
        else // complex return via ptr (pCppReturn)
        {
            pCppReturn = *(void **)pCppStack;
            pCppStack += sizeof(void *);

            pUnoReturn = (bridges::cpp_uno::shared::relatesToInterfaceType(
                              pReturnTypeDescr )
                          ? alloca( pReturnTypeDescr->nSize )
                          : pCppReturn); // direct way
        }
    }

    // stack space
    OSL_ENSURE( sizeof(void *) == sizeof(sal_Int32), "### unexpected size!" );
    // parameters
    void ** pUnoArgs = (void **)alloca( 4 * sizeof(void *) * nParams );
    void ** pCppArgs = pUnoArgs + nParams;
    // indizes of values this have to be converted (interface conversion cpp<=>uno)
    sal_Int32 * pTempIndizes = (sal_Int32 *)(pUnoArgs + (2 * nParams));
    // type descriptions for reconversions
    typelib_TypeDescription ** ppTempParamTypeDescr = (typelib_TypeDescription **)(pUnoArgs + (3 * nParams));

    sal_Int32 nTempIndizes = 0;

    for ( sal_Int32 nPos = 0; nPos < nParams; ++nPos )
    {
        const typelib_MethodParameter & rParam = pParams[nPos];
        typelib_TypeDescription * pParamTypeDescr = 0;
        TYPELIB_DANGER_GET( &pParamTypeDescr, rParam.pTypeRef );

        if (!rParam.bOut
            && bridges::cpp_uno::shared::isSimpleType( pParamTypeDescr ))
            // value
        {
            pCppArgs[nPos] = pCppStack;
            pUnoArgs[nPos] = pCppStack;
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
            else if (bridges::cpp_uno::shared::relatesToInterfaceType(
                         pParamTypeDescr ))
            {
                ::uno_copyAndConvertData(
                    pUnoArgs[nPos] = alloca( pParamTypeDescr->nSize ),
                    *(void **)pCppStack, pParamTypeDescr,
                    pThis->getBridge()->getCpp2Uno() );
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
    (*pThis->getUnoI()->pDispatcher)(
        pThis->getUnoI(), pMemberTypeDescr, pUnoReturn, pUnoArgs, &pUnoExc );

    // in case an exception occured...
    if (pUnoExc)
    {
        // destruct temporary in/inout params
        while (nTempIndizes--)
        {
            sal_Int32 nIndex = pTempIndizes[nTempIndizes];

            if (pParams[nIndex].bIn) // is in/inout => was constructed
            {
                ::uno_destructData( pUnoArgs[nIndex], ppTempParamTypeDescr[nTempIndizes], 0 );
            }
            TYPELIB_DANGER_RELEASE( ppTempParamTypeDescr[nTempIndizes] );
        }
        if (pReturnTypeDescr)
        {
            TYPELIB_DANGER_RELEASE( pReturnTypeDescr );
        }

        CPPU_CURRENT_NAMESPACE::msci_raiseException(
            &aUnoExc, pThis->getBridge()->getUno2Cpp() );
            // has to destruct the any
        // is here for dummy
        return typelib_TypeClass_VOID;
    }
    else // else no exception occured...
    {
        // temporary params
        while (nTempIndizes--)
        {
            sal_Int32 nIndex = pTempIndizes[nTempIndizes];
            typelib_TypeDescription * pParamTypeDescr = ppTempParamTypeDescr[nTempIndizes];

            if (pParams[nIndex].bOut) // inout/out
            {
                // convert and assign
                ::uno_destructData(
                    pCppArgs[nIndex], pParamTypeDescr, cpp_release );
                ::uno_copyAndConvertData(
                    pCppArgs[nIndex], pUnoArgs[nIndex], pParamTypeDescr,
                    pThis->getBridge()->getUno2Cpp() );
            }
            // destroy temp uno param
            ::uno_destructData( pUnoArgs[nIndex], pParamTypeDescr, 0 );

            TYPELIB_DANGER_RELEASE( pParamTypeDescr );
        }
        // return
        if (pCppReturn) // has complex return
        {
            if (pUnoReturn != pCppReturn) // needs reconversion
            {
                ::uno_copyAndConvertData(
                    pCppReturn, pUnoReturn, pReturnTypeDescr,
                    pThis->getBridge()->getUno2Cpp() );
                // destroy temp uno return
                ::uno_destructData(
                    pUnoReturn, pReturnTypeDescr, 0 );
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
static typelib_TypeClass __cdecl cpp_mediate(
    void ** pCallStack, sal_Int32 nFunctionIndex, sal_Int32 nVtableOffset,
    sal_Int64 * pRegisterReturn /* space for register return */ )
{
    OSL_ENSURE( sizeof(sal_Int32)==sizeof(void *), "### unexpected!" );

    // pCallStack: ret adr, this, [ret *], params
    void * pThis = static_cast< char * >(pCallStack[1]) - nVtableOffset;
    bridges::cpp_uno::shared::CppInterfaceProxy * pCppI
        = bridges::cpp_uno::shared::CppInterfaceProxy::castInterfaceToProxy(
            pThis);

    typelib_InterfaceTypeDescription * pTypeDescr = pCppI->getTypeDescr();
    OSL_ENSURE( nFunctionIndex < pTypeDescr->nMapFunctionIndexToMemberIndex,
                 "### illegal vtable index!" );
    if (nFunctionIndex >= pTypeDescr->nMapFunctionIndexToMemberIndex)
    {
        throw RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("illegal vtable index!") ),
                                (XInterface *)pThis );
    }

    // determine called method
    sal_Int32 nMemberPos = pTypeDescr->pMapFunctionIndexToMemberIndex[nFunctionIndex];
    OSL_ENSURE( nMemberPos < pTypeDescr->nAllMembers, "### illegal member index!" );

    TypeDescription aMemberDescr( pTypeDescr->ppAllMembers[nMemberPos] );

    typelib_TypeClass eRet;
    switch (aMemberDescr.get()->eTypeClass)
    {
    case typelib_TypeClass_INTERFACE_ATTRIBUTE:
    {
        if (pTypeDescr->pMapMemberIndexToFunctionIndex[nMemberPos] == nFunctionIndex)
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
        switch (nFunctionIndex)
        {
            // standard XInterface vtable calls
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
            TYPELIB_DANGER_GET( &pTD, reinterpret_cast< Type * >( pCallStack[3] )->getTypeLibType() );
            if (pTD)
            {
                XInterface * pInterface = 0;
                (*pCppI->getBridge()->getCppEnv()->getRegisteredInterface)(
                    pCppI->getBridge()->getCppEnv(),
                    (void **)&pInterface, pCppI->getOid().pData,
                    (typelib_InterfaceTypeDescription *)pTD );

                if (pInterface)
                {
                    ::uno_any_construct(
                        reinterpret_cast< uno_Any * >( pCallStack[2] ),
                        &pInterface, pTD, cpp_acquire );
                    pInterface->release();
                    TYPELIB_DANGER_RELEASE( pTD );
                    *(void **)pRegisterReturn = pCallStack[2];
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
        throw RuntimeException(
            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("no member description found!") ),
            (XInterface *)pThis );
        // is here for dummy
        eRet = typelib_TypeClass_VOID;
    }
    }

    return eRet;
}

//==================================================================================================
/**
 * is called on incoming vtable calls
 * (called by asm snippets)
 */
static __declspec(naked) void __cdecl cpp_vtable_call(void)
{
__asm
    {
        sub		esp, 8		// space for immediate return type
        push	esp
        push    edx         // vtable offset
        push	eax			// function index
        mov		eax, esp
        add		eax, 20
        push	eax			// original stack ptr

        call	cpp_mediate
        add		esp, 16

        cmp		eax, typelib_TypeClass_FLOAT
        je		Lfloat
        cmp		eax, typelib_TypeClass_DOUBLE
        je		Ldouble
        cmp		eax, typelib_TypeClass_HYPER
        je		Lhyper
        cmp		eax, typelib_TypeClass_UNSIGNED_HYPER
        je		Lhyper
        // rest is eax
        pop		eax
        add		esp, 4
        ret
Lhyper:
        pop		eax
        pop		edx
        ret
Lfloat:
        fld		dword ptr [esp]
        add		esp, 8
        ret
Ldouble:
        fld		qword ptr [esp]
        add		esp, 8
        ret
    }
}

//==================================================================================================
int const codeSnippetSize = 16;

unsigned char * codeSnippet(
    unsigned char * code, sal_Int32 functionIndex, sal_Int32 vtableOffset)
{
    unsigned char * p = code;
    OSL_ASSERT(sizeof (sal_Int32) == 4);
    // mov eax, functionIndex:
    *p++ = 0xB8;
    *reinterpret_cast< sal_Int32 * >(p) = functionIndex;
    p += sizeof (sal_Int32);
    // mov edx, vtableOffset:
    *p++ = 0xBA;
    *reinterpret_cast< sal_Int32 * >(p) = vtableOffset;
    p += sizeof (sal_Int32);
    // jmp rel32 cpp_vtable_call:
    *p++ = 0xE9;
    *reinterpret_cast< sal_Int32 * >(p)
        = ((unsigned char *) cpp_vtable_call) - p - sizeof (sal_Int32);
    p += sizeof (sal_Int32);
    OSL_ASSERT(p - code <= codeSnippetSize);
    return code + codeSnippetSize;
}

}

void ** bridges::cpp_uno::shared::VtableFactory::mapBlockToVtable(char * block)
{
    return reinterpret_cast< void ** >(block) + 1;
}

char * bridges::cpp_uno::shared::VtableFactory::createBlock(
    sal_Int32 slotCount, void *** slots)
{
    struct Rtti {
        sal_Int32 n0, n1, n2;
        type_info * rtti;
        Rtti():
            n0(0), n1(0), n2(0),
            rtti(CPPU_CURRENT_NAMESPACE::msci_getRTTI(
                     rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
                                       "com.sun.star.uno.XInterface"))))
        {}
    };
    static Rtti rtti;

    char * block = new char[
        (slotCount + 1) * sizeof (void *) + slotCount * codeSnippetSize];
    *slots = mapBlockToVtable(block);
    (*slots)[-1] = &rtti;
    return block;
}

unsigned char * bridges::cpp_uno::shared::VtableFactory::addLocalFunctions(
    void ** slots, unsigned char * code,
    typelib_InterfaceTypeDescription const * type, sal_Int32 functionOffset,
    sal_Int32 functionCount, sal_Int32 vtableOffset)
{
    for (sal_Int32 i = 0; i < functionCount; ++i) {
        *slots++ = code;
        code = codeSnippet(code, functionOffset++, vtableOffset);
    }
    return code;
}

void bridges::cpp_uno::shared::VtableFactory::flushCode(
    unsigned char const * begin, unsigned char const * end)
{
    DWORD old_protect;
#if OSL_DEBUG_LEVEL > 0
    BOOL success =
#endif
    VirtualProtect( const_cast<unsigned char *>(begin), end - begin,
                    PAGE_EXECUTE_READWRITE, &old_protect );
    OSL_ENSURE( success, "VirtualProtect() failed!" );
}
