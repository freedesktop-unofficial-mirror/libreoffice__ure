/*************************************************************************
 *
 *  $RCSfile: Sequence.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: dbo $ $Date: 2000-09-25 14:49:22 $
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
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#define _COM_SUN_STAR_UNO_SEQUENCE_HXX_

#ifndef _CPPU_MACROS_HXX_
#include <cppu/macros.hxx>
#endif

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _OSL_INTERLCK_H_
#include <osl/interlck.h>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_H_
#include <com/sun/star/uno/Sequence.h>
#endif
#ifndef _TYPELIB_TYPEDESCRIPTION_H_
#include <typelib/typedescription.h>
#endif
#ifndef _UNO_DATA_H_
#include <uno/data.h>
#endif
#ifndef _COM_SUN_STAR_UNO_GENFUNC_HXX_
#include <com/sun/star/uno/genfunc.hxx>
#endif


/** */ //for docpp
namespace com
{     
/** */ //for docpp
namespace sun
{     
/** */ //for docpp
namespace star
{     
/** */ //for docpp
namespace uno
{     

//__________________________________________________________________________________________________
template< class E >
inline Sequence< E >::Sequence()
{
    const Type & rType = ::getCppuType( this );
    ::uno_type_sequence_construct( &_pSequence, rType.getTypeLibType(), 0, 0, cpp_acquire );
}
//__________________________________________________________________________________________________
template< class E >
inline Sequence< E >::Sequence( const Sequence< E > & rSeq )
{
    ::osl_incrementInterlockedCount( &rSeq._pSequence->nRefCount );
    _pSequence = rSeq._pSequence;
}
//__________________________________________________________________________________________________
template< class E >
inline Sequence< E >::Sequence( const E * pElements, sal_Int32 len )
    : _pSequence( 0 )
{
    const Type & rType = ::getCppuType( this );
    ::uno_type_sequence_construct( &_pSequence, rType.getTypeLibType(),
                                   const_cast< E * >( pElements ), len, cpp_acquire );
}
//__________________________________________________________________________________________________
template< class E >
inline Sequence< E >::Sequence( sal_Int32 len )
    : _pSequence( 0 )
{
    const Type & rType = ::getCppuType( this );
    ::uno_type_sequence_construct( &_pSequence, rType.getTypeLibType(), 0, len, cpp_acquire );
}
//__________________________________________________________________________________________________
template< class E >
inline Sequence< E >::~Sequence()
{
    const Type & rType = ::getCppuType( this );
    ::uno_type_destructData( this, rType.getTypeLibType(), cpp_release );
}
//__________________________________________________________________________________________________
template< class E >
inline Sequence< E > & Sequence< E >::operator = ( const Sequence< E > & rSeq )
{
    const Type & rType = ::getCppuType( this );
    ::uno_type_sequence_assign( &_pSequence, rSeq._pSequence, rType.getTypeLibType(), cpp_release );
    return *this;
}
//__________________________________________________________________________________________________
template< class E >
inline sal_Bool Sequence< E >::operator == ( const Sequence< E > & rSeq ) const
{
    if (_pSequence == rSeq._pSequence)
        return sal_True;
    const Type & rType = ::getCppuType( this );
    return ::uno_type_equalData( const_cast< Sequence< E > * >( this ), rType.getTypeLibType(),
                                 const_cast< Sequence< E > * >( &rSeq ), rType.getTypeLibType(),
                                 cpp_queryInterface, cpp_release );
}
//__________________________________________________________________________________________________
template< class E >
inline E * Sequence< E >::getArray()
{
    const Type & rType = ::getCppuType( this );
    ::uno_type_sequence_reference2One( &_pSequence, rType.getTypeLibType(), cpp_acquire, cpp_release );
    return reinterpret_cast< E * >( _pSequence->elements );
}
//__________________________________________________________________________________________________
template< class E >
inline E & Sequence< E >::operator [] ( sal_Int32 nIndex )
{
    OSL_ENSURE( nIndex >= 0 && nIndex < getLength(), "### illegal index of sequence!" );
    return getArray()[ nIndex ];
}
//__________________________________________________________________________________________________
template< class E >
inline const E & Sequence< E >::operator [] ( sal_Int32 nIndex ) const
{
    OSL_ENSURE( nIndex >= 0 && nIndex < getLength(), "### illegal index of sequence!" );
    return getConstArray()[ nIndex ];
}
//__________________________________________________________________________________________________
template< class E >
inline void Sequence< E >::realloc( sal_Int32 nSize )
{
    const Type & rType = ::getCppuType( this );
    ::uno_type_sequence_realloc( &_pSequence, rType.getTypeLibType(), nSize, cpp_acquire, cpp_release );
}

//--------------------------------------------------------------------------------------------------
inline ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL toUnoSequence(
    const ::rtl::ByteSequence & rByteSequence )
{
    return ::com::sun::star::uno::Sequence< sal_Int8 >(
        * reinterpret_cast< const ::com::sun::star::uno::Sequence< sal_Int8 > * >( &rByteSequence ) );
}

}
}
}
}

// generic sequence template
template< class S >
#if (defined(__SUNPRO_CC) && (__SUNPRO_CC == 0x500)) || (defined(__GNUC__) && defined(__APPLE__))
inline const ::com::sun::star::uno::Type
#else
inline const ::com::sun::star::uno::Type &
#endif
SAL_CALL getCppuType( const ::com::sun::star::uno::Sequence< S > * )
{
#if (defined(__SUNPRO_CC) && (__SUNPRO_CC == 0x500)) || (defined(__GNUC__) && defined(__APPLE__))
    typelib_TypeDescriptionReference * s_pType = 0;
#else
    static typelib_TypeDescriptionReference * s_pType = 0;
    if (! s_pType)
    {
#endif
        const ::com::sun::star::uno::Type & rElementType = ::getCppuType(
            (::com::sun::star::uno::Sequence< S >::ElementType *)0 );
        ::typelib_static_sequence_type_init( &s_pType, rElementType.getTypeLibType() );
#if !( (defined(__SUNPRO_CC) && (__SUNPRO_CC == 0x500)) || (defined(__GNUC__) && defined(__APPLE__)))
    }
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >( &s_pType );
#else
    return ::com::sun::star::uno::Type( s_pType );
#endif
}

// char sequence
#if (defined(__SUNPRO_CC) && (__SUNPRO_CC == 0x500)) || (defined(__GNUC__) && defined(__APPLE__))
inline const ::com::sun::star::uno::Type
#else
inline const ::com::sun::star::uno::Type &
#endif
SAL_CALL getCharSequenceCppuType()
{
#if (defined(__SUNPRO_CC) && (__SUNPRO_CC == 0x500)) || (defined(__GNUC__) && defined(__APPLE__))
    typelib_TypeDescriptionReference * s_pType = 0;
#else
    static typelib_TypeDescriptionReference * s_pType = 0;
    if (! s_pType)
    {
#endif
        const ::com::sun::star::uno::Type & rElementType = ::getCharCppuType();
        ::typelib_static_sequence_type_init( &s_pType, rElementType.getTypeLibType() );
#if ! ((defined(__SUNPRO_CC) && (__SUNPRO_CC == 0x500)) || (defined(__GNUC__) && defined(__APPLE__)))
    }
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >( &s_pType );
#else
    return ::com::sun::star::uno::Type( s_pType );
#endif
}

#endif
