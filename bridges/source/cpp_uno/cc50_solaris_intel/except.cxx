/*************************************************************************
 *
 *  $RCSfile: except.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: kz $ $Date: 2005-07-01 12:15:52 $
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

#include <cstddef>
#include <dlfcn.h>
#include <new.h>
#include <typeinfo>
#include <list>
#include <map>
#ifndef _RTL_ALLOC_H_
#include <rtl/alloc.h>
#endif
#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

#include <rtl/strbuf.hxx>

#ifndef _TYPELIB_TYPEDESCRIPTION_HXX_
#include <typelib/typedescription.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif

#include "bridges/cpp_uno/shared/arraypointer.hxx"

#include "cc50_solaris_intel.hxx"

#include <hash.cxx>

// need a += operator for OString and sal_Char
namespace rtl
{
    inline OString& operator+=( OString& rString, sal_Char cAdd )
    {
        sal_Char add[2];
        add[0] = cAdd;
        add[1] = 0;
        return rString += add;
    }
}

using namespace std;
using namespace osl;
using namespace rtl;
using namespace com::sun::star::uno;

namespace CPPU_CURRENT_NAMESPACE
{

static OString toUNOname( const OString & rRTTIname )
{
    OString aRet;

    const sal_Char* pRTTI = rRTTIname.getStr();
    const sal_Char* pOrg  = pRTTI;
    const sal_Char* pLast = pRTTI;

    while( 1 )
    {
        if( *pRTTI == ':' || ! *pRTTI )
        {
            if( aRet.getLength() )
                aRet += ".";
            aRet += rRTTIname.copy( pLast - pOrg, pRTTI - pLast );
            while( *pRTTI == ':' )
                pRTTI++;
            pLast = pRTTI;
            if( ! *pRTTI )
                break;
        }
        else
            pRTTI++;
    }

    return aRet;
}
//==================================================================================================
static OString toRTTIname( const OString & rUNOname )
{
    OStringBuffer aRet( rUNOname.getLength()*2 );

    sal_Int32 nIndex = 0;
    do
    {
        if( nIndex > 0 )
            aRet.append( "::" );
        aRet.append( rUNOname.getToken( 0, '.', nIndex ) );
    } while( nIndex != -1 );

    return aRet.makeStringAndClear();
}
//==================================================================================================

static OString toRTTImangledname( const OString & rRTTIname )
{
    if( ! rRTTIname.getLength() )
        return OString();

    OStringBuffer aRet( rRTTIname.getLength()*2 );

    aRet.append( "__1n" );
    sal_Int32 nIndex = 0;
    do
    {
        OString aToken( rRTTIname.getToken( 0, ':', nIndex ) );
        int nBytes = aToken.getLength();
        if( nBytes )
        {
            if( nBytes  > 25 )
            {
                aRet.append( (sal_Char)( nBytes/26 + 'a' ) );
                aRet.append( (sal_Char)( nBytes%26 + 'A' ) );
            }
            else
                aRet.append( (sal_Char)( nBytes + 'A' ) );
            aRet.append( aToken );
        }
    } while( nIndex != -1 );

    aRet.append( '_' );

    return aRet.makeStringAndClear();
}

//##################################################################################################
//#### RTTI simulation #############################################################################
//##################################################################################################

class RTTIHolder
{
    std::map< OString, void* > aAllRTTI;
public:
    ~RTTIHolder();

    void* getRTTI( const OString& rTypename );
    void* getRTTI_UnoName( const OString& rUnoTypename )
        { return getRTTI( toRTTIname( rUnoTypename ) ); }

    void* insertRTTI( const OString& rTypename );
    void* insertRTTI_UnoName( const OString& rTypename )
        { return insertRTTI( toRTTIname( rTypename ) ); }
    void* generateRTTI( typelib_CompoundTypeDescription* pCompTypeDescr );
};

RTTIHolder::~RTTIHolder()
{
    for ( std::map< OString, void* >::const_iterator iPos( aAllRTTI.begin() );
          iPos != aAllRTTI.end(); ++iPos )
    {
        delete[] static_cast< char * >(iPos->second);
    }
}

#if OSL_DEBUG_LEVEL > 1
#include <stdio.h>
#endif

void* RTTIHolder::getRTTI( const OString& rTypename )
{
    std::map< OString, void* >::iterator element;

    element = aAllRTTI.find( rTypename );
    if( element != aAllRTTI.end() )
        return (*element).second;

    // create rtti structure
    element = aAllRTTI.find( rTypename );
    if( element != aAllRTTI.end() )
        return (*element).second;

    return NULL;
}

static long nMagicId = 1;

void* RTTIHolder::insertRTTI( const OString& rTypename )
{
    OString aMangledName( toRTTImangledname( rTypename ) );
    NIST_Hash aHash( aMangledName.getStr(), aMangledName.getLength() );


    // rSuperTypename MUST exist !!!
    std::size_t const RTTI_SIZE = 19; // 14???
    void** pRTTI = reinterpret_cast< void ** >(
        new char[RTTI_SIZE * sizeof (void *) + strlen(rTypename.getStr()) + 1]);
    pRTTI[  0 ] = reinterpret_cast< void * >(RTTI_SIZE * sizeof (void *));
    pRTTI[  1 ] = NULL;
    pRTTI[  2 ] = (void*)(7*sizeof(void*));
    pRTTI[  3 ] = (void*)aHash.getHash()[0];
    pRTTI[  4 ] = (void*)aHash.getHash()[1];
    pRTTI[  5 ] = (void*)aHash.getHash()[2];
    pRTTI[  6 ] = (void*)aHash.getHash()[3];
    pRTTI[  7 ] = NULL;
    pRTTI[  8 ] = NULL;

    pRTTI[  9 ] = pRTTI[ 3 ];
    pRTTI[ 10 ] = pRTTI[ 4 ];
    pRTTI[ 11 ] = pRTTI[ 5 ];
    pRTTI[ 12 ] = pRTTI[ 6 ];
    pRTTI[ 13 ] = (void*)0x80000000;
    strcpy(reinterpret_cast< char * >(pRTTI + RTTI_SIZE), rTypename.getStr());

    aAllRTTI[ rTypename ] = (void*)pRTTI;
#if OSL_DEBUG_LEVEL > 1
    fprintf( stderr,
             "generating base RTTI for type %s:\n"
             "   mangled: %s\n"
             "   hash: %.8x %.8x %.8x %.8x\n",
             rTypename.getStr(),
             aMangledName.getStr(),
             pRTTI[ 3 ], pRTTI[ 4 ], pRTTI[ 5 ], pRTTI[ 6 ]
             );
#endif
    return pRTTI;
}

void* RTTIHolder::generateRTTI( typelib_CompoundTypeDescription * pCompTypeDescr )
{
    OString aUNOCompTypeName( OUStringToOString( pCompTypeDescr->aBase.pTypeName, RTL_TEXTENCODING_ASCII_US ) );
    OString aRTTICompTypeName( toRTTIname( aUNOCompTypeName ) );

    void* pHaveRTTI = getRTTI( aRTTICompTypeName );
    if( pHaveRTTI )
        return pHaveRTTI;

    if( ! pCompTypeDescr->pBaseTypeDescription )
        // this is a base type
        return insertRTTI( aRTTICompTypeName );

    // get base class RTTI
    void* pSuperRTTI = generateRTTI( pCompTypeDescr->pBaseTypeDescription );
    OSL_ENSURE( pSuperRTTI, "could not generate RTTI for supertype !" );

    // find out the size to allocate for RTTI
    void** pInherit = (void**)((sal_uInt32)pSuperRTTI + ((sal_uInt32*)pSuperRTTI)[2] + 8);
    int nInherit;
    for( nInherit = 1; pInherit[ nInherit*5-1 ] != (void*)0x80000000; nInherit++ )
        ;

    OString aMangledName( toRTTImangledname( aRTTICompTypeName ) );
    NIST_Hash aHash( aMangledName.getStr(), aMangledName.getLength() );

    std::size_t const rttiSize = 14 + nInherit * 5;
    void** pRTTI = reinterpret_cast< void ** >(
        new char[
            rttiSize * sizeof (void *)
            + strlen(aRTTICompTypeName.getStr()) + 1]);
    pRTTI[  0 ] = reinterpret_cast< void * >(rttiSize * sizeof (void *));
    pRTTI[  1 ] = NULL;
    pRTTI[  2 ] = (void*)(7*sizeof(void*));
    pRTTI[  3 ] = (void*)aHash.getHash()[0];
    pRTTI[  4 ] = (void*)aHash.getHash()[1];
    pRTTI[  5 ] = (void*)aHash.getHash()[2];
    pRTTI[  6 ] = (void*)aHash.getHash()[3];
    pRTTI[  7 ] = NULL;
    pRTTI[  8 ] = NULL;

    memcpy( pRTTI+9, pInherit, 4*nInherit*5 );
    pRTTI[ 8 +nInherit*5 ] = NULL;
    pRTTI[ 9 +nInherit*5 ] = pRTTI[ 3 ];
    pRTTI[ 10+nInherit*5 ] = pRTTI[ 4 ];
    pRTTI[ 11+nInherit*5 ] = pRTTI[ 5 ];
    pRTTI[ 12+nInherit*5 ] = pRTTI[ 6 ];
    pRTTI[ 13+nInherit*5 ] = (void*)0x80000000;
    strcpy(
        reinterpret_cast< char * >(pRTTI + rttiSize),
        aRTTICompTypeName.getStr());

    aAllRTTI[ aRTTICompTypeName ] = (void*)pRTTI;

#if OSL_DEBUG_LEVEL > 1
    fprintf( stderr,
             "generating struct RTTI for type %s:\n"
             "   mangled: %s\n"
             "   hash: %.8x %.8x %.8X %.8x\n",
             aRTTICompTypeName.getStr(),
             aMangledName.getStr(),
             pRTTI[ 3 ], pRTTI[ 4 ], pRTTI[ 5 ], pRTTI[ 6 ]
             );
#endif

    return pRTTI;
}

//__________________________________________________________________________________________________

static void deleteException(
    void* pExc, unsigned char* thunk, typelib_TypeDescription* pType )
{
     uno_destructData( pExc, pType, cpp_release );
     typelib_typedescription_release( pType );
    delete[] thunk;
}

//__________________________________________________________________________________________________

//##################################################################################################
//#### exported ####################################################################################
//##################################################################################################

void cc50_solaris_intel_raiseException( uno_Any * pUnoExc, uno_Mapping * pUno2Cpp )
{
#if OSL_DEBUG_LEVEL > 1
    OString cstr(
        OUStringToOString(
            *reinterpret_cast< OUString const * >( &pUnoExc->pType->pTypeName ),
            RTL_TEXTENCODING_ASCII_US ) );
    fprintf( stderr, "> uno exception occured: %s\n", cstr.getStr() );
#endif
    bridges::cpp_uno::shared::ArrayPointer< unsigned char > thunkPtr(
        new unsigned char[24]);
    typelib_TypeDescription * pTypeDescr = 0;
    // will be released by deleteException
    typelib_typedescriptionreference_getDescription( &pTypeDescr, pUnoExc->pType );

    void* pRTTI;
    {
    static ::osl::Mutex aMutex;
    ::osl::Guard< ::osl::Mutex > guard( aMutex );

    static RTTIHolder * s_pRTTI = 0;
    if (! s_pRTTI)
    {
#ifdef LEAK_STATIC_DATA
        s_pRTTI = new RTTIHolder();
#else
        static RTTIHolder s_aRTTI;
        s_pRTTI = &s_aRTTI;
#endif
    }

    pRTTI = s_pRTTI->generateRTTI( (typelib_CompoundTypeDescription *)pTypeDescr );
    }

    // a must be
    OSL_ENSURE( sizeof(sal_Int32) == sizeof(void *), "### pointer size differs from sal_Int32!" );

    void * pCppExc = __Crun::ex_alloc( pTypeDescr->nSize );
    uno_copyAndConvertData( pCppExc, pUnoExc->pData, pTypeDescr, pUno2Cpp );

    // destruct uno exception
    uno_any_destruct( pUnoExc, 0 );

    unsigned char * thunk = thunkPtr.release();
    // movl %esp, %ecx:
    thunk[0] = 0x8B;
    thunk[1] = 0xCC;
    // pushl pTypeDescr:
    thunk[2] = 0x68;
    *reinterpret_cast< void ** >(thunk + 3) = pTypeDescr;
    // pushl thunk:
    thunk[7] = 0x68;
    *reinterpret_cast< void ** >(thunk + 8) = thunk;
    // pushl 4(%ecx):
    thunk[12] = 0xFF;
    thunk[13] = 0x71;
    thunk[14] = 0x04;
    // call deleteException:
    thunk[15] = 0xE8;
    *reinterpret_cast< std::ptrdiff_t * >(thunk + 16) =
        reinterpret_cast< unsigned char * >(deleteException) - (thunk + 20);
    // addl $12, %esp:
    thunk[20] = 0x83;
    thunk[21] = 0xC4;
    thunk[22] = 0x0C;
    // ret:
    thunk[23] = 0xC3;

    __Crun::ex_throw(
        pCppExc, (const __Crun::static_type_info*)pRTTI,
        reinterpret_cast< void (*)(void *) >(thunk) );
}

void cc50_solaris_intel_fillUnoException(
    void* pCppExc,
    const char* pInfo,
    uno_Any* pUnoExc,
    uno_Mapping * pCpp2Uno )
{
    OSL_ASSERT( pInfo != 0 );
    OString uno_name( toUNOname( pInfo ) );
    OUString aName( OStringToOUString(
                        uno_name, RTL_TEXTENCODING_ASCII_US ) );
    typelib_TypeDescription * pExcTypeDescr = 0;
    typelib_typedescription_getByName( &pExcTypeDescr, aName.pData );

    if (pExcTypeDescr == 0) // the thing that should not be
    {
        RuntimeException aRE(
            OUString( RTL_CONSTASCII_USTRINGPARAM(
                          "exception type not found: ") ) + aName,
            Reference< XInterface >() );
        Type const & rType = ::getCppuType( &aRE );
        uno_type_any_constructAndConvert(
            pUnoExc, &aRE, rType.getTypeLibType(), pCpp2Uno );
#if OSL_DEBUG_LEVEL > 0
        OString cstr( OUStringToOString(
                          aRE.Message, RTL_TEXTENCODING_ASCII_US ) );
        OSL_ENSURE( 0, cstr.getStr() );
#endif
        return;
    }

#if OSL_DEBUG_LEVEL > 1
    fprintf( stderr, "> c++ exception occured: %s\n",
             ::rtl::OUStringToOString(
                 pExcTypeDescr->pTypeName,
                 RTL_TEXTENCODING_ASCII_US ).getStr() );
#endif
    // construct uno exception any
    uno_any_constructAndConvert(
        pUnoExc, pCppExc, pExcTypeDescr, pCpp2Uno );
    typelib_typedescription_release( pExcTypeDescr );
}

}

