/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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

#include <malloc.h>

#include <com/sun/star/uno/genfunc.hxx>
#include <uno/data.h>
#include <typelib/typedescription.hxx>

#include "bridges/cpp_uno/shared/bridge.hxx"
#include "bridges/cpp_uno/shared/cppinterfaceproxy.hxx"
#include "bridges/cpp_uno/shared/types.hxx"
#include "bridges/cpp_uno/shared/vtablefactory.hxx"

#include "mscx.hxx"

// #define DEBUG_WITH_JUST_MESSAGEBOXES

using namespace ::com::sun::star::uno;

//==================================================================================================
static inline typelib_TypeClass cpp2uno_call(
    bridges::cpp_uno::shared::CppInterfaceProxy * pThis,
    const typelib_TypeDescription * pMemberTypeDescr,
    typelib_TypeDescriptionReference * pReturnTypeRef, // NULL indicates void return
    sal_Int32 nParams, typelib_MethodParameter * pParams,
    void ** pStack )
{
    // Return type
    typelib_TypeDescription * pReturnTypeDescr = NULL;
    if ( pReturnTypeRef )
        TYPELIB_DANGER_GET( &pReturnTypeDescr, pReturnTypeRef );

    int nFirstRealParam = 2;

    void * pUnoReturn = NULL;
    void * pCppReturn = NULL;   // Complex return ptr: if != NULL && != pUnoReturn, reconversion need

    if ( pReturnTypeDescr )
    {
        if ( !bridges::cpp_uno::shared::isSimpleType( pReturnTypeDescr ) )
        {
            pCppReturn = pStack[nFirstRealParam++];

            pUnoReturn = ( bridges::cpp_uno::shared::relatesToInterfaceType( pReturnTypeDescr )
                           ? alloca( pReturnTypeDescr->nSize )
                           : pCppReturn ); // direct way
        }
        else // complex return, store it directly where the C++ called wants it
        {
            pUnoReturn = pStack;
        }
    }

    void ** pCppIncomingParams = pStack + nFirstRealParam;

    // Unlike this method for other archs, prefer clarity to
    // micro-optimization, and allocate these array separately

    // Parameters passed to the UNO function
    void ** pUnoArgs = (void **)alloca( sizeof(void *) * nParams );

    // Parameters received from C++
    void ** pCppArgs = (void **)alloca( sizeof(void *) * nParams );

    // Indexes of values this have to be converted (interface conversion cpp<=>uno)
    int * pTempIndexes =
        (int *)alloca( sizeof(int) * nParams );

    // Type descriptions for reconversions
    typelib_TypeDescription ** ppTempParamTypeDescr =
        (typelib_TypeDescription **)alloca( sizeof(void *) * nParams );

    int nTempIndexes = 0;

    for ( int nPos = 0; nPos < nParams; ++nPos )
    {
        const typelib_MethodParameter & rParam = pParams[nPos];

        if ( !rParam.bOut
             && bridges::cpp_uno::shared::isSimpleType( rParam.pTypeRef ) )
        {
            pCppArgs[nPos] = pUnoArgs[nPos] = pCppIncomingParams++;
        }
        else // ptr to complex value | ref
        {
            typelib_TypeDescription * pParamTD = NULL;
            TYPELIB_DANGER_GET( &pParamTD, rParam.pTypeRef );

            void * pCppStack;

            pCppArgs[nPos] = pCppStack = *pCppIncomingParams++;

            if ( !rParam.bIn ) // Pure out
            {
                // UNO out is unconstructed mem
                pUnoArgs[nPos] = alloca( pParamTD->nSize );
                pTempIndexes[nTempIndexes] = nPos;
                // pParamTD will be released at reconversion
                ppTempParamTypeDescr[nTempIndexes++] = pParamTD;
            }
            //
            else if ( bridges::cpp_uno::shared::relatesToInterfaceType(
                         pParamTD ) )
            {
                ::uno_copyAndConvertData(
                    pUnoArgs[nPos] = alloca( pParamTD->nSize ),
                    pCppStack, pParamTD,
                    pThis->getBridge()->getCpp2Uno() );
                pTempIndexes[nTempIndexes] = nPos; // Has to be reconverted
                // pParamTD will be released at reconversion
                ppTempParamTypeDescr[nTempIndexes++] = pParamTD;
            }
            else // direct way
            {
                pUnoArgs[nPos] = pCppStack;
                // No longer needed
                TYPELIB_DANGER_RELEASE( pParamTD );
            }
        }
    }

    // ExceptionHolder
    uno_Any aUnoExc; // Any will be constructed by callee
    uno_Any * pUnoExc = &aUnoExc;

    // invoke UNO dispatch call
    (*pThis->getUnoI()->pDispatcher)(
        pThis->getUnoI(), pMemberTypeDescr, pUnoReturn, pUnoArgs, &pUnoExc );

    // in case an exception occurred...
    if ( pUnoExc )
    {
        // Destruct temporary in/inout params
        while ( nTempIndexes-- )
        {
            int nIndex = pTempIndexes[nTempIndexes];

            if ( pParams[nIndex].bIn ) // Is in/inout => was constructed
            {
                ::uno_destructData( pUnoArgs[nIndex], ppTempParamTypeDescr[nTempIndexes], 0 );
            }
            TYPELIB_DANGER_RELEASE( ppTempParamTypeDescr[nTempIndexes] );
        }
        if ( pReturnTypeDescr )
            TYPELIB_DANGER_RELEASE( pReturnTypeDescr );

        CPPU_CURRENT_NAMESPACE::mscx_raiseException(
            &aUnoExc, pThis->getBridge()->getUno2Cpp() ); // Has to destruct the any

        // Is here for dummy
        return typelib_TypeClass_VOID;
    }
    else // Else, no exception occurred...
    {
        // Temporary params
        while (nTempIndexes--)
        {
            int nIndex = pTempIndexes[nTempIndexes];
            typelib_TypeDescription * pParamTypeDescr = ppTempParamTypeDescr[nTempIndexes];

            if ( pParams[nIndex].bOut ) // inout/out
            {
                // Convert and assign
                ::uno_destructData(
                    pCppArgs[nIndex], pParamTypeDescr, cpp_release );
                ::uno_copyAndConvertData(
                    pCppArgs[nIndex], pUnoArgs[nIndex], pParamTypeDescr,
                    pThis->getBridge()->getUno2Cpp() );
            }
            // Destroy temp uno param
            ::uno_destructData( pUnoArgs[nIndex], pParamTypeDescr, 0 );

            TYPELIB_DANGER_RELEASE( pParamTypeDescr );
        }
        // Return
        if ( pCppReturn ) // Has complex return
        {
            if ( pUnoReturn != pCppReturn ) // Needs reconversion
            {
                ::uno_copyAndConvertData(
                    pCppReturn, pUnoReturn, pReturnTypeDescr,
                    pThis->getBridge()->getUno2Cpp() );
                // Destroy temp uno return
                ::uno_destructData( pUnoReturn, pReturnTypeDescr, 0 );
            }
            // Complex return ptr is set to eax
            pStack[0] = pCppReturn;
        }
        if ( pReturnTypeDescr )
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
extern "C" typelib_TypeClass cpp_vtable_call(
    sal_Int32 nFunctionIndex,
    sal_Int32 nVtableOffset,
    void ** pStack )
{
    // pStack points to space for return value, after which
    // follows our return address (uninteresting) then the spilled
    // integer or floating-point register parameters from the call to
    // the trampoline, followed by stack parameters. Note that if the
    // callee returns a large value, the first parameter is actually a
    // pointer to where it should store its return value. The first
    // "real" parameter is the "this" pointer.

    void * pThis;
    if ( nFunctionIndex & 0x80000000 )
    {
        nFunctionIndex &= 0x7fffffff;
        pThis = pStack[3];
    }
    else
    {
        pThis = pStack[2];
    }
    pThis = static_cast<char *>( pThis ) - nVtableOffset;

    bridges::cpp_uno::shared::CppInterfaceProxy * pCppI =
        bridges::cpp_uno::shared::CppInterfaceProxy::castInterfaceToProxy( pThis );

    typelib_InterfaceTypeDescription * pTypeDescr = pCppI->getTypeDescr();

    OSL_ENSURE( nFunctionIndex < pTypeDescr->nMapFunctionIndexToMemberIndex, "### illegal vtable index!\n" );
    if ( nFunctionIndex >= pTypeDescr->nMapFunctionIndexToMemberIndex )
    {
        throw RuntimeException( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("illegal vtable index!")),
                                reinterpret_cast<XInterface *>( pCppI ) );
    }

    // determine called method
    sal_Int32 nMemberPos = pTypeDescr->pMapFunctionIndexToMemberIndex[nFunctionIndex];
    OSL_ENSURE( nMemberPos < pTypeDescr->nAllMembers, "### illegal member index!\n" );

    TypeDescription aMemberDescr( pTypeDescr->ppAllMembers[nMemberPos] );

    typelib_TypeClass eRet;
    switch ( aMemberDescr.get()->eTypeClass )
    {
        case typelib_TypeClass_INTERFACE_ATTRIBUTE:
        {
            typelib_TypeDescriptionReference *pAttrTypeRef =
                reinterpret_cast<typelib_InterfaceAttributeTypeDescription *>( aMemberDescr.get() )->pAttributeTypeRef;

            if ( pTypeDescr->pMapMemberIndexToFunctionIndex[nMemberPos] == nFunctionIndex )
            {
                // is GET method
                eRet = cpp2uno_call( pCppI, aMemberDescr.get(), pAttrTypeRef,
                        0, NULL, // no params
                        pStack );
            }
            else
            {
                // is SET method
                typelib_MethodParameter aParam;
                aParam.pTypeRef = pAttrTypeRef;
                aParam.bIn		= sal_True;
                aParam.bOut		= sal_False;

                eRet = cpp2uno_call( pCppI, aMemberDescr.get(),
                        0, // indicates void return
                        1, &aParam,
                        pStack );
            }
            break;
        }
        case typelib_TypeClass_INTERFACE_METHOD:
        {
            // is METHOD
            switch ( nFunctionIndex )
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
                    typelib_TypeDescription * pTD = NULL;
                    // the incoming C++ parameters are: The hidden return value pointer,
                    // the this pointer, and then the actual queryInterface() only parameter.
                    // Thus pStack[4], the third parameter.
                    TYPELIB_DANGER_GET( &pTD, reinterpret_cast<Type *>( pStack[4] )->getTypeLibType() );

                    if ( pTD )
                    {
                        XInterface * pInterface = NULL;
                        (*pCppI->getBridge()->getCppEnv()->getRegisteredInterface)
                            ( pCppI->getBridge()->getCppEnv(),
                              (void **)&pInterface,
                              pCppI->getOid().pData,
                              reinterpret_cast<typelib_InterfaceTypeDescription *>( pTD ) );

                        if ( pInterface )
                        {
                            // pStack[0] = hidden return value pointer
                            ::uno_any_construct( reinterpret_cast<uno_Any *>( pStack[0] ),
                                                 &pInterface, pTD, cpp_acquire );

                            pInterface->release();
                            TYPELIB_DANGER_RELEASE( pTD );

                            eRet = typelib_TypeClass_ANY;
                            break;
                        }
                        TYPELIB_DANGER_RELEASE( pTD );
                    }
                } // Fall through!
                default:
                {
                    typelib_InterfaceMethodTypeDescription *pMethodTD =
                        reinterpret_cast<typelib_InterfaceMethodTypeDescription *>( aMemberDescr.get() );

                    eRet = cpp2uno_call( pCppI, aMemberDescr.get(),
                                         pMethodTD->pReturnTypeRef,
                                         pMethodTD->nParams,
                                         pMethodTD->pParams,
                                         pStack );
                }
            }
            break;
        }
        default:
        {
            throw RuntimeException( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("no member description found!")),
                                    reinterpret_cast<XInterface *>( pCppI ) );
            // is here for dummy
            eRet = typelib_TypeClass_VOID;
        }
    }

    return eRet;
}

//==================================================================================================
extern "C" {

// These are actually addresses in the code compiled from codeSnippet.asm
extern char
    fp_spill_templates,
    fp_spill_templates_end,
    trampoline_template,
    trampoline_template_function_table,
    trampoline_template_spill_params,
    trampoline_template_spill_params_end,
    trampoline_template_function_index,
    trampoline_template_vtable_offset,
    trampoline_template_cpp_vtable_call,
    trampoline_template_end;
}

// Just the code
int const codeSnippetSize =
    (int) (&trampoline_template_end - &trampoline_template);

// This function generates the code that acts as a proxy for the UNO function to be called.
// The generated code does the following:
// - Save register parametrs.

unsigned char * codeSnippet(
    unsigned char * code,
    char param_kind[4],
    sal_Int32 functionIndex,
    sal_Int32 vtableOffset,
    bool bHasHiddenParam )
{
    OSL_ASSERT( (&fp_spill_templates_end - &fp_spill_templates) ==
                (&trampoline_template_spill_params_end - &trampoline_template_spill_params) );

    OSL_ASSERT( ((&fp_spill_templates_end - &fp_spill_templates) / 4) * 4 ==
                (&fp_spill_templates_end - &fp_spill_templates) );

    if ( bHasHiddenParam )
        functionIndex |= 0x80000000;

    int const one_spill_instruction_size =
        (int) ((&fp_spill_templates_end - &fp_spill_templates)) / 4;

    memcpy( code, &trampoline_template, codeSnippetSize );

    for (int i = 0; i < 4; ++i)
    {
        if ( param_kind[i] == CPPU_CURRENT_NAMESPACE::REGPARAM_FLT )
        {
            memcpy (&trampoline_template_spill_params + i*one_spill_instruction_size,
                    &fp_spill_templates + i*one_spill_instruction_size,
                    one_spill_instruction_size);
        }
    }

    ((sal_uInt64*) (code + (&trampoline_template_function_index
                            - &trampoline_template)))[-1] =
        functionIndex;
    ((sal_uInt64*) (code + (&trampoline_template_vtable_offset
                            - &trampoline_template)))[-1] =
        vtableOffset;
    ((void**) (code + (&trampoline_template_cpp_vtable_call
                            - &trampoline_template)))[-1] =
        cpp_vtable_call;

    // Add unwind data for the dynamically generated function by
    // calling RtlAddFunctionTable().

    // The unwind data is inside the function code, at a fixed offset
    // from the function start. See codeSnippet.asm. Actually this is
    // unnecessarily complex, we could as well just allocate the
    // function table dynamically. But it doesn't hurt either, I
    // think.

    // The real problem now is that we need to remove the unwind data
    // with RtlDeleteFunctionTable() in freeExec() in
    // vtablefactory.cxx. See comment there.

    RUNTIME_FUNCTION *pFunTable =
        (RUNTIME_FUNCTION *) (code + (&trampoline_template_function_table
                                      - &trampoline_template));

    RtlAddFunctionTable( pFunTable, 1, (DWORD64) code );

    return code + codeSnippetSize;
}

struct bridges::cpp_uno::shared::VtableFactory::Slot { void * fn; };

bridges::cpp_uno::shared::VtableFactory::Slot *
bridges::cpp_uno::shared::VtableFactory::mapBlockToVtable(void * block)
{
    return static_cast< Slot * >(block) + 1;
}

sal_Size bridges::cpp_uno::shared::VtableFactory::getBlockSize(
    sal_Int32 slotCount)
{
    return (slotCount + 1) * sizeof (Slot) + slotCount * codeSnippetSize;
}

bridges::cpp_uno::shared::VtableFactory::Slot *
bridges::cpp_uno::shared::VtableFactory::initializeBlock(
    void * block, sal_Int32 slotCount)
{
    struct Rtti {
        sal_Int32 n0, n1, n2;
        type_info * rtti;
        Rtti():
            n0(0), n1(0), n2(0),
            rtti(CPPU_CURRENT_NAMESPACE::mscx_getRTTI(
                     rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
                                       "com.sun.star.uno.XInterface"))))
        {}
    };
    static Rtti rtti;

    Slot * slots = mapBlockToVtable(block);
    slots[-1].fn = &rtti;
    return slots + slotCount;
}

#ifdef DEBUG_WITH_JUST_MESSAGEBOXES

static void whatthefuck(sal_Int64 i, ...)
{
    MessageBoxA (NULL, "Shit!", "whatthefuck in cpp2uno.cxx", MB_OK);
}

#endif

unsigned char * bridges::cpp_uno::shared::VtableFactory::addLocalFunctions(
    Slot ** slots, unsigned char * code,
    typelib_InterfaceTypeDescription const * type, sal_Int32 functionOffset,
    sal_Int32 functionCount, sal_Int32 vtableOffset)
{
    (*slots) -= functionCount;
    Slot * s = *slots;

    for (int member = 0; member < type->nMembers; ++member) {
        typelib_TypeDescription * pTD = NULL;

        TYPELIB_DANGER_GET( &pTD, type->ppMembers[ member ] );
        OSL_ASSERT( pTD );

        char param_kind[4];
        int nr = 0;

        for (int i = 0; i < 4; ++i)
            param_kind[i] = CPPU_CURRENT_NAMESPACE::REGPARAM_INT;

        if ( pTD->eTypeClass == typelib_TypeClass_INTERFACE_ATTRIBUTE )
        {
            typelib_InterfaceAttributeTypeDescription *pAttrTD =
                reinterpret_cast<typelib_InterfaceAttributeTypeDescription *>( pTD );

            // Getter

#ifndef DEBUG_WITH_JUST_MESSAGEBOXES
            (s++)->fn = code;
            code = codeSnippet( code, param_kind, functionOffset++, vtableOffset,
                                pTD->nSize > 8);
#else
            (s++)->fn = whatthefuck;
#endif

            if ( ! pAttrTD->bReadOnly )
            {
                // Setter
#ifndef DEBUG_WITH_JUST_MESSAGEBOXES
                (s++)->fn = code;
                code = codeSnippet( code, param_kind, functionOffset++, vtableOffset, false );
#else
                (s++)->fn = whatthefuck;
#endif
            }
        }
        else if ( pTD->eTypeClass == typelib_TypeClass_INTERFACE_METHOD )
        {
            typelib_InterfaceMethodTypeDescription *pMethodTD =
                reinterpret_cast<typelib_InterfaceMethodTypeDescription *>( pTD );

            typelib_TypeDescription *pReturnTD = NULL;
            TYPELIB_DANGER_GET( &pReturnTD, pMethodTD->pReturnTypeRef );
            OSL_ASSERT( pReturnTD );

            if ( pReturnTD->nSize > 8 )
            {
                // Hidden return value
                ++nr;
            }

            // 'this'
            ++nr;

            for (int param = 0; nr < 4 && param < pMethodTD->nParams; ++param, ++nr)
            {
                typelib_TypeDescription *pParamTD = NULL;

                TYPELIB_DANGER_GET( &pParamTD, pMethodTD->pParams[param].pTypeRef );
                OSL_ASSERT( pParamTD );

                if ( pParamTD->eTypeClass == typelib_TypeClass_FLOAT ||
                     pParamTD->eTypeClass == typelib_TypeClass_DOUBLE )
                    param_kind[nr] = CPPU_CURRENT_NAMESPACE::REGPARAM_FLT;

                TYPELIB_DANGER_RELEASE( pParamTD );
            }

#ifndef DEBUG_WITH_JUST_MESSAGEBOXES
            (s++)->fn = code;
            code = codeSnippet( code, param_kind, functionOffset++, vtableOffset,
                                pReturnTD->nSize > 8);
#else
            (s++)->fn = whatthefuck;
#endif

            TYPELIB_DANGER_RELEASE( pReturnTD );
        }
        else
            OSL_ASSERT( false );

        TYPELIB_DANGER_RELEASE( pTD );
    }
    return code;
}

void bridges::cpp_uno::shared::VtableFactory::flushCode(
    unsigned char const *, unsigned char const *)
{}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
