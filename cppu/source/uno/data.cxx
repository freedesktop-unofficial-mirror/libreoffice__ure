/*************************************************************************
 *
 *  $RCSfile: data.cxx,v $
 *
 *  $Revision: 1.20 $
 *
 *  last change: $Author: dbo $ $Date: 2002-10-23 10:40:57 $
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

#include "constr.hxx"
#include "destr.hxx"
#include "copy.hxx"
#include "assign.hxx"
#include "eq.hxx"

using namespace cppu;
using namespace rtl;
using namespace com::sun::star::uno;


namespace cppu
{

uno_Sequence g_emptySeq = { 1, 0, { 0 } }; // static empty sequence
typelib_TypeDescriptionReference * g_pVoidType = 0;
typelib_TypeDescription * g_pQITD = 0;

//==================================================================================================
void defaultConstructStruct(
    void * pMem,
    typelib_CompoundTypeDescription * pCompType )
    SAL_THROW( () )
{
    _defaultConstructStruct( pMem, pCompType );
}
//==================================================================================================
void copyConstructStruct(
    void * pDest, void * pSource,
    typelib_CompoundTypeDescription * pTypeDescr,
    uno_AcquireFunc acquire, uno_Mapping * mapping )
    SAL_THROW( () )
{
    _copyConstructStruct( pDest, pSource, pTypeDescr, acquire, mapping );
}
//==================================================================================================
void destructStruct(
    void * pValue,
    typelib_CompoundTypeDescription * pTypeDescr,
    uno_ReleaseFunc release )
    SAL_THROW( () )
{
    _destructStruct( pValue, pTypeDescr, release );
}
//==================================================================================================
sal_Bool equalStruct(
    void * pDest, void *pSource,
    typelib_CompoundTypeDescription * pTypeDescr,
    uno_QueryInterfaceFunc queryInterface, uno_ReleaseFunc release )
    SAL_THROW( () )
{
    return _equalStruct( pDest, pSource, pTypeDescr, queryInterface, release );
}
//==================================================================================================
sal_Bool assignStruct(
    void * pDest, void * pSource,
    typelib_CompoundTypeDescription * pTypeDescr,
    uno_QueryInterfaceFunc queryInterface, uno_AcquireFunc acquire, uno_ReleaseFunc release )
    SAL_THROW( () )
{
    return _assignStruct( pDest, pSource, pTypeDescr, queryInterface, acquire, release );
}
//==================================================================================================
void copyConstructSequence(
    uno_Sequence ** ppDest, uno_Sequence * pSource,
    typelib_TypeDescriptionReference * pElementType,
    uno_AcquireFunc acquire, uno_Mapping * mapping )
    SAL_THROW( () )
{
    _copyConstructSequence( ppDest, pSource, pElementType, acquire, mapping );
}
//==================================================================================================
void destructSequence(
    uno_Sequence ** ppSequence,
    typelib_TypeDescriptionReference * pElementType,
    uno_ReleaseFunc release )
    SAL_THROW( () )
{
    uno_Sequence * pSequence = *ppSequence;
    OSL_ASSERT( pSequence );
    if (! ::osl_decrementInterlockedCount( &pSequence->nRefCount ))
    {
        if (pSequence->nElements)
        {
            _destructElements(
                pSequence->elements, pElementType,
                0, pSequence->nElements, release );
        }
        ::rtl_freeMemory( pSequence );
    }
}
//==================================================================================================
sal_Bool equalSequence(
    uno_Sequence * pDest, uno_Sequence * pSource,
    typelib_TypeDescriptionReference * pElementType,
    uno_QueryInterfaceFunc queryInterface, uno_ReleaseFunc release )
    SAL_THROW( () )
{
    return _equalSequence( pDest, pSource, pElementType, queryInterface, release );
}

extern "C"
{
//##################################################################################################
void SAL_CALL uno_type_constructData(
    void * pMem, typelib_TypeDescriptionReference * pType )
    SAL_THROW_EXTERN_C()
{
    _defaultConstructData( pMem, pType, 0 );
}
//##################################################################################################
void SAL_CALL uno_constructData(
    void * pMem, typelib_TypeDescription * pTypeDescr )
    SAL_THROW_EXTERN_C()
{
    _defaultConstructData( pMem, pTypeDescr->pWeakRef, pTypeDescr );
}
//##################################################################################################
void SAL_CALL uno_type_destructData(
    void * pValue, typelib_TypeDescriptionReference * pType,
    uno_ReleaseFunc release )
    SAL_THROW_EXTERN_C()
{
    _destructData( pValue, pType, 0, release );
}
//##################################################################################################
void SAL_CALL uno_destructData(
    void * pValue,
    typelib_TypeDescription * pTypeDescr,
    uno_ReleaseFunc release )
    SAL_THROW_EXTERN_C()
{
    _destructData( pValue, pTypeDescr->pWeakRef, pTypeDescr, release );
}
//##################################################################################################
void SAL_CALL uno_type_copyData(
    void * pDest, void * pSource, 
    typelib_TypeDescriptionReference * pType,
    uno_AcquireFunc acquire )
    SAL_THROW_EXTERN_C()
{
    _copyConstructData( pDest, pSource, pType, 0, acquire, 0 );
}
//##################################################################################################
void SAL_CALL uno_copyData(
    void * pDest, void * pSource,
    typelib_TypeDescription * pTypeDescr,
    uno_AcquireFunc acquire )
    SAL_THROW_EXTERN_C()
{
    _copyConstructData( pDest, pSource, pTypeDescr->pWeakRef, pTypeDescr, acquire, 0 );
}
//##################################################################################################
void SAL_CALL uno_type_copyAndConvertData(
    void * pDest, void * pSource,
    typelib_TypeDescriptionReference * pType,
    uno_Mapping * mapping )
    SAL_THROW_EXTERN_C()
{
    _copyConstructData( pDest, pSource, pType, 0, 0, mapping );
}
//##################################################################################################
void SAL_CALL uno_copyAndConvertData(
    void * pDest, void * pSource,
    typelib_TypeDescription * pTypeDescr,
    uno_Mapping * mapping )
    SAL_THROW_EXTERN_C()
{
    _copyConstructData( pDest, pSource, pTypeDescr->pWeakRef, pTypeDescr, 0, mapping );
}
//##################################################################################################
sal_Bool SAL_CALL uno_type_equalData(
    void * pVal1, typelib_TypeDescriptionReference * pVal1Type,
    void * pVal2, typelib_TypeDescriptionReference * pVal2Type,
    uno_QueryInterfaceFunc queryInterface, uno_ReleaseFunc release )
    SAL_THROW_EXTERN_C()
{
    return _equalData(
        pVal1, pVal1Type, 0,
        pVal2, pVal2Type, 0,
        queryInterface, release );
}
//##################################################################################################
sal_Bool SAL_CALL uno_equalData(
    void * pVal1, typelib_TypeDescription * pVal1TD,
    void * pVal2, typelib_TypeDescription * pVal2TD,
    uno_QueryInterfaceFunc queryInterface, uno_ReleaseFunc release )
    SAL_THROW_EXTERN_C()
{
    return _equalData(
        pVal1, pVal1TD->pWeakRef, pVal1TD,
        pVal2, pVal2TD->pWeakRef, pVal2TD,
        queryInterface, release );
}
//##################################################################################################
sal_Bool SAL_CALL uno_type_assignData(
    void * pDest, typelib_TypeDescriptionReference * pDestType,
    void * pSource, typelib_TypeDescriptionReference * pSourceType,
    uno_QueryInterfaceFunc queryInterface, uno_AcquireFunc acquire, uno_ReleaseFunc release )
    SAL_THROW_EXTERN_C()
{
    return _assignData(
        pDest, pDestType, 0,
        pSource, pSourceType, 0,
        queryInterface, acquire, release );
}
//##################################################################################################
sal_Bool SAL_CALL uno_assignData(
    void * pDest, typelib_TypeDescription * pDestTD,
    void * pSource, typelib_TypeDescription * pSourceTD,
    uno_QueryInterfaceFunc queryInterface, uno_AcquireFunc acquire, uno_ReleaseFunc release )
    SAL_THROW_EXTERN_C()
{
    return _assignData(
        pDest, pDestTD->pWeakRef, pDestTD,
        pSource, pSourceTD->pWeakRef, pSourceTD,
        queryInterface, acquire, release );
}
//##################################################################################################
sal_Bool SAL_CALL uno_type_isAssignableFromData(
    typelib_TypeDescriptionReference * pAssignable,
    void * pFrom, typelib_TypeDescriptionReference * pFromType,
    uno_QueryInterfaceFunc queryInterface, uno_ReleaseFunc release )
    SAL_THROW_EXTERN_C()
{
    if (::typelib_typedescriptionreference_isAssignableFrom( pAssignable, pFromType ))
        return sal_True;
    if (typelib_TypeClass_INTERFACE != pFromType->eTypeClass ||
        typelib_TypeClass_INTERFACE != pAssignable->eTypeClass)
    {
        return sal_False;
    }
    
    // query
    if (!pFrom)
        return sal_False;
    void * pInterface = *(void **)pFrom;
    if (! pInterface)
        return sal_False;
    
    if (queryInterface)
    {
        void * p = (*queryInterface)( pInterface, pAssignable );
        if (p)
        {
            (*release)( p );
        }
        return (0 != p);
    }
    else /* bin UNO */
    {
        uno_Any aRet, aExc;
        uno_Any * pExc = &aExc;
            
        void * aArgs[1];
        aArgs[0] = &pAssignable;
            
        typelib_TypeDescription * pMTqueryInterface = _getQueryInterfaceTypeDescr();
        (*((uno_Interface *)pInterface)->pDispatcher)(
            (uno_Interface *)pInterface, pMTqueryInterface, &aRet, aArgs, &pExc );
        ::typelib_typedescription_release( pMTqueryInterface );
            
        OSL_ENSURE( !pExc, "### Exception occured during queryInterface()!" );
        if (pExc)
        {
            _destructAny( pExc, 0 );
            return sal_False;
        }
        else
        {
            sal_Bool ret = (typelib_TypeClass_INTERFACE == aRet.pType->eTypeClass);
            _destructAny( &aRet, 0 );
            return ret;
        }
    }
}
}


//##################################################################################################
//##################################################################################################
//##################################################################################################


#ifdef _DEBUG

#include <stdio.h>

#ifdef SAL_W32
#	pragma pack(push, 8)
#elif defined(SAL_OS2)
#	pragma pack(8)
#endif

#if defined(__GNUC__) && defined(LINUX) && defined(INTEL)
#define MAX_ALIGNMENT_4
#endif

#define OFFSET_OF( s, m ) ((sal_Size)((char *)&((s *)16)->m -16))

#define BINTEST_VERIFY( c ) \
    if (! (c)) { fprintf( stderr, "### binary compatibility test failed: " #c " [line %d]!!!\n", __LINE__ ); abort(); }
#define BINTEST_VERIFYOFFSET( s, m, n ) \
    if (OFFSET_OF(s, m) != n) { fprintf( stderr, "### OFFSET_OF(" #s ", "  #m ") = %d instead of expected %d!!!\n", OFFSET_OF(s, m), n ); abort(); }

#ifdef CPPU_ASSERTIONS
#if defined(__GNUC__) && defined(LINUX) && (defined(INTEL) || defined(POWERPC) || defined(X86_64))
#define BINTEST_VERIFYSIZE( s, n ) \
    fprintf( stderr, "> sizeof(" #s ") = %d; __alignof__ (" #s ") = %d\n", sizeof(s), __alignof__ (s) ); \
    if (sizeof(s) != n) { fprintf( stderr, "### sizeof(" #s ") = %d instead of expected %d!!!\n", sizeof(s), n ); abort(); }
#else // ! GNUC
#define BINTEST_VERIFYSIZE( s, n ) \
    fprintf( stderr, "> sizeof(" #s ") = %d\n", sizeof(s) ); \
    if (sizeof(s) != n) { fprintf( stderr, "### sizeof(" #s ") = %d instead of expected %d!!!\n", sizeof(s), n ); abort(); }
#endif
#else // ! CPPU_ASSERTIONS
#define BINTEST_VERIFYSIZE( s, n ) \
    if (sizeof(s) != n) { fprintf( stderr, "### sizeof(" #s ") = %d instead of expected %d!!!\n", sizeof(s), n ); abort(); }
#endif

struct C1
{
    sal_Int16 n1;
};
struct C2 : public C1
{
    sal_Int32 n2 CPPU_GCC3_ALIGN( C1 );
};
struct C3 : public C2
{
    double d3;
    sal_Int32 n3;
};
struct C4 : public C3
{
    sal_Int32 n4 CPPU_GCC3_ALIGN( C3 );
    double d4;
};
struct C5 : public C4
{
    sal_Int64 n5;
    sal_Bool b5;
};
struct C6 : public C1
{
    C5 c6 CPPU_GCC3_ALIGN( C1 );
    sal_Bool b6;
};

struct D
{
    sal_Int16 d;
    sal_Int32 e;
};
struct E
{
    sal_Bool a;
    sal_Bool b;
    sal_Bool c;
    sal_Int16 d;
    sal_Int32 e;
};

struct M
{ 
    sal_Int32	n;
    sal_Int16	o;
};

struct N : public M
{ 
    sal_Int16	p CPPU_GCC3_ALIGN( M );
};
struct N2
{
    M m;
    sal_Int16	p;
};

struct O : public M
{ 
    double	p;
};
struct O2 : public O
{ 
    double	p2;
};

struct P : public N
{ 
    double	p2;
};

struct empty
{
};
struct second : public empty
{
    int a;
};

struct AlignSize_Impl
{
    sal_Int16	nInt16;
    double		dDouble;
};

struct Char1
{
    char c1;
};
struct Char2 : public Char1
{
    char c2 CPPU_GCC3_ALIGN( Char1 );
};
struct Char3 : public Char2
{
    char c3 CPPU_GCC3_ALIGN( Char2 );
};
struct Char4
{
    Char3 chars;
    char c;
};

class BinaryCompatible_Impl
{
public:
    BinaryCompatible_Impl();
};
BinaryCompatible_Impl::BinaryCompatible_Impl()
{
#ifdef MAX_ALIGNMENT_4
    // max alignment is 4
    BINTEST_VERIFYOFFSET( AlignSize_Impl, dDouble, 4 );
    BINTEST_VERIFYSIZE( AlignSize_Impl, 12 );
#else
    // max alignment is 8
    BINTEST_VERIFYOFFSET( AlignSize_Impl, dDouble, 8 );
    BINTEST_VERIFYSIZE( AlignSize_Impl, 16 );
#endif
    
    // sequence
    BINTEST_VERIFY( (SAL_SEQUENCE_HEADER_SIZE % 8) == 0 );
    // enum
    BINTEST_VERIFY( sizeof( TypeClass ) == sizeof( sal_Int32 ) );
    // any
    BINTEST_VERIFY( sizeof(void *) >= sizeof(sal_Int32) );
    BINTEST_VERIFY( sizeof( Any ) == sizeof( uno_Any ) );
    BINTEST_VERIFY( sizeof( Any ) == sizeof( void * ) * 3 );
    BINTEST_VERIFYOFFSET( Any, pType, 0 );
    BINTEST_VERIFYOFFSET( Any, pData, 1 * sizeof (void *) );
    BINTEST_VERIFYOFFSET( Any, pReserved, 2 * sizeof (void *) );
    // interface
    BINTEST_VERIFY( sizeof( Reference< XInterface > ) == sizeof( XInterface * ) );
    // string
    BINTEST_VERIFY( sizeof( OUString ) == sizeof( rtl_uString * ) );
    // struct
    BINTEST_VERIFYSIZE( M, 8 );
    BINTEST_VERIFYOFFSET( M, o, 4 );
    BINTEST_VERIFYSIZE( N, 12 );
    BINTEST_VERIFYOFFSET( N, p, 8 );
    BINTEST_VERIFYSIZE( N2, 12 );
    BINTEST_VERIFYOFFSET( N2, p, 8 );
    BINTEST_VERIFYSIZE( O, 16 );
    BINTEST_VERIFYSIZE( D, 8 );
    BINTEST_VERIFYOFFSET( D, e, 4 );
    BINTEST_VERIFYOFFSET( E, d, 4 );
    BINTEST_VERIFYOFFSET( E, e, 8 );

    BINTEST_VERIFYSIZE( C1, 2 );
    BINTEST_VERIFYSIZE( C2, 8 );
    BINTEST_VERIFYOFFSET( C2, n2, 4 );

#ifdef MAX_ALIGNMENT_4
    BINTEST_VERIFYSIZE( C3, 20 );
    BINTEST_VERIFYOFFSET( C3, d3, 8 );
    BINTEST_VERIFYOFFSET( C3, n3, 16 );
    BINTEST_VERIFYSIZE( C4, 32 );
    BINTEST_VERIFYOFFSET( C4, n4, 20 );
    BINTEST_VERIFYOFFSET( C4, d4, 24 );
    BINTEST_VERIFYSIZE( C5, 44 );
    BINTEST_VERIFYOFFSET( C5, n5, 32 );
    BINTEST_VERIFYOFFSET( C5, b5, 40 );
    BINTEST_VERIFYSIZE( C6, 52 );
    BINTEST_VERIFYOFFSET( C6, c6, 4 );
    BINTEST_VERIFYOFFSET( C6, b6, 48 );
#else
    BINTEST_VERIFYSIZE( C3, 24 );
    BINTEST_VERIFYOFFSET( C3, d3, 8 );
    BINTEST_VERIFYOFFSET( C3, n3, 16 );
    BINTEST_VERIFYSIZE( C4, 40 );
    BINTEST_VERIFYOFFSET( C4, n4, 24 );
    BINTEST_VERIFYOFFSET( C4, d4, 32 );
    BINTEST_VERIFYSIZE( C5, 56 );
    BINTEST_VERIFYOFFSET( C5, n5, 40 );
    BINTEST_VERIFYOFFSET( C5, b5, 48 );
    BINTEST_VERIFYSIZE( C6, 72 );
    BINTEST_VERIFYOFFSET( C6, c6, 8 );
    BINTEST_VERIFYOFFSET( C6, b6, 64 );
#endif

    BINTEST_VERIFYSIZE( O2, 24 );    
    BINTEST_VERIFYOFFSET( O2, p2, 16 );
    
    BINTEST_VERIFYSIZE( Char3, 3 );
    BINTEST_VERIFYOFFSET( Char4, c, 3 );
    
#ifdef MAX_ALIGNMENT_4
    // max alignment is 4
    BINTEST_VERIFYSIZE( P, 20 );
#else
    // alignment of P is 8, because of P[] ...
    BINTEST_VERIFYSIZE( P, 24 );
    BINTEST_VERIFYSIZE( second, sizeof( int ) );
#endif
}

#ifdef SAL_W32
#	pragma pack(pop)
#elif defined(SAL_OS2)
#	pragma pack()
#endif

static BinaryCompatible_Impl aTest;

#endif

}
