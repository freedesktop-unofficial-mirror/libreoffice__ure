/*************************************************************************
 *
 *  $RCSfile: Type.hxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: rt $ $Date: 2004-07-23 14:39:40 $
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
#ifndef _COM_SUN_STAR_UNO_TYPE_HXX_
#define _COM_SUN_STAR_UNO_TYPE_HXX_

#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_TYPE_H_
#include <com/sun/star/uno/Type.h>
#endif


namespace com
{     
namespace sun
{     
namespace star
{     
namespace uno
{

//__________________________________________________________________________________________________
inline Type::Type() SAL_THROW( () )
{
    _pType = reinterpret_cast< const ::com::sun::star::uno::Type * >(
        ::typelib_static_type_getByTypeClass( typelib_TypeClass_VOID ) )->getTypeLibType();
    ::typelib_typedescriptionreference_acquire( _pType );
}
//__________________________________________________________________________________________________
inline Type::Type( TypeClass eTypeClass, const ::rtl::OUString & rTypeName ) SAL_THROW( () )
    : _pType( 0 )
{
    ::typelib_typedescriptionreference_new( &_pType, (typelib_TypeClass)eTypeClass, rTypeName.pData );
}
//__________________________________________________________________________________________________
inline Type::Type( TypeClass eTypeClass, const sal_Char * pTypeName ) SAL_THROW( () )
    : _pType( 0 )
{
    ::typelib_typedescriptionreference_newByAsciiName( &_pType, (typelib_TypeClass)eTypeClass, pTypeName );
}
//__________________________________________________________________________________________________
inline Type::Type( typelib_TypeDescriptionReference * pType ) SAL_THROW( () )
    : _pType( pType )
{
    ::typelib_typedescriptionreference_acquire( _pType );
}
//__________________________________________________________________________________________________
inline Type::Type( typelib_TypeDescriptionReference * pType, UnoType_NoAcquire ) SAL_THROW( () )
    : _pType( pType )
{
}
//__________________________________________________________________________________________________
inline Type::Type( typelib_TypeDescriptionReference * pType, __sal_NoAcquire ) SAL_THROW( () )
    : _pType( pType )
{
}
//__________________________________________________________________________________________________
inline Type::Type( const Type & rType ) SAL_THROW( () )
    : _pType( rType._pType )
{
    ::typelib_typedescriptionreference_acquire( _pType );
}
//__________________________________________________________________________________________________
inline ::rtl::OUString Type::getTypeName() const SAL_THROW( () )
{
    return ::rtl::OUString( _pType->pTypeName );
}
//__________________________________________________________________________________________________
inline Type & Type::operator = ( const Type & rType ) SAL_THROW( () )
{
    ::typelib_typedescriptionreference_assign( &_pType, rType._pType );
    return *this;
}

//__________________________________________________________________________________________________
template< class T >
typelib_TypeDescriptionReference * Array< T >::s_pType = 0;

}
}
}
}

inline const ::com::sun::star::uno::Type & SAL_CALL getCppuType( const ::com::sun::star::uno::Type * ) SAL_THROW( () )
{
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        ::typelib_static_type_getByTypeClass( typelib_TypeClass_TYPE ) );
}

inline const ::com::sun::star::uno::Type & SAL_CALL getCppuVoidType() SAL_THROW( () )
{
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        ::typelib_static_type_getByTypeClass( typelib_TypeClass_VOID ) );
}
inline const ::com::sun::star::uno::Type & SAL_CALL getVoidCppuType() SAL_THROW( () )
{
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        ::typelib_static_type_getByTypeClass( typelib_TypeClass_VOID ) );
}

inline const ::com::sun::star::uno::Type & SAL_CALL getCppuBooleanType() SAL_THROW( () )
{
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        ::typelib_static_type_getByTypeClass( typelib_TypeClass_BOOLEAN ) );
}
inline const ::com::sun::star::uno::Type & SAL_CALL getBooleanCppuType() SAL_THROW( () )
{
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        ::typelib_static_type_getByTypeClass( typelib_TypeClass_BOOLEAN ) );
}
inline const ::com::sun::star::uno::Type & SAL_CALL getCppuType( const sal_Bool * ) SAL_THROW( () )
{
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        ::typelib_static_type_getByTypeClass( typelib_TypeClass_BOOLEAN ) );
}

inline const ::com::sun::star::uno::Type & SAL_CALL getCppuType(
    bool const * ) SAL_THROW( () )
{
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        ::typelib_static_type_getByTypeClass( typelib_TypeClass_BOOLEAN ) );
}

inline const ::com::sun::star::uno::Type & SAL_CALL getCharCppuType() SAL_THROW( () )
{
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        ::typelib_static_type_getByTypeClass( typelib_TypeClass_CHAR ) );
}
inline const ::com::sun::star::uno::Type & SAL_CALL getCppuCharType() SAL_THROW( () )
{
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        ::typelib_static_type_getByTypeClass( typelib_TypeClass_CHAR ) );
}

inline const ::com::sun::star::uno::Type & SAL_CALL getCppuType( const sal_Int8 * ) SAL_THROW( () )
{
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        ::typelib_static_type_getByTypeClass( typelib_TypeClass_BYTE ) );
}

inline const ::com::sun::star::uno::Type & SAL_CALL getCppuType( const ::rtl::OUString * ) SAL_THROW( () )
{
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        ::typelib_static_type_getByTypeClass( typelib_TypeClass_STRING ) );
}

inline const ::com::sun::star::uno::Type & SAL_CALL getCppuType( const sal_Int16 * ) SAL_THROW( () )
{
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        ::typelib_static_type_getByTypeClass( typelib_TypeClass_SHORT ) );
}

inline const ::com::sun::star::uno::Type & SAL_CALL getCppuType( const sal_uInt16 * ) SAL_THROW( () )
{
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        ::typelib_static_type_getByTypeClass( typelib_TypeClass_UNSIGNED_SHORT ) );
}

inline const ::com::sun::star::uno::Type & SAL_CALL getCppuType( const sal_Int32 * ) SAL_THROW( () )
{
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        ::typelib_static_type_getByTypeClass( typelib_TypeClass_LONG ) );
}

inline const ::com::sun::star::uno::Type & SAL_CALL getCppuType( const sal_uInt32 * ) SAL_THROW( () )
{
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        ::typelib_static_type_getByTypeClass( typelib_TypeClass_UNSIGNED_LONG ) );
}

inline const ::com::sun::star::uno::Type & SAL_CALL getCppuType( const sal_Int64 * ) SAL_THROW( () )
{
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        ::typelib_static_type_getByTypeClass( typelib_TypeClass_HYPER ) );
}

inline const ::com::sun::star::uno::Type & SAL_CALL getCppuType( const sal_uInt64 * ) SAL_THROW( () )
{
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        ::typelib_static_type_getByTypeClass( typelib_TypeClass_UNSIGNED_HYPER ) );
}

inline const ::com::sun::star::uno::Type & SAL_CALL getCppuType( const float * ) SAL_THROW( () )
{
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        ::typelib_static_type_getByTypeClass( typelib_TypeClass_FLOAT ) );
}

inline const ::com::sun::star::uno::Type & SAL_CALL getCppuType( const double * ) SAL_THROW( () )
{
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        ::typelib_static_type_getByTypeClass( typelib_TypeClass_DOUBLE ) );
}

template< class T >
inline const ::com::sun::star::uno::Type & SAL_CALL getCppuArrayType1( T * pT ) SAL_THROW( () )
{
    if (! ::com::sun::star::uno::Array< T >::s_pType)
    {
        const ::com::sun::star::uno::Type & rElementType = ::getCppuType( *pT );
        sal_Int32 size = sizeof( **pT );
        sal_Int32 dim1 = sizeof( *pT ) / size;
        ::typelib_static_array_type_init(
            & ::com::sun::star::uno::Array< T >::s_pType, rElementType.getTypeLibType(),
            1, dim1 );
    }
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        & ::com::sun::star::uno::Array< T >::s_pType );
}

template< class T >
inline const ::com::sun::star::uno::Type & SAL_CALL getCppuArrayType2( T * pT ) SAL_THROW( () )
{
    if (! ::com::sun::star::uno::Array< T >::s_pType)
    {
        const ::com::sun::star::uno::Type & rElementType = ::getCppuType( **pT );
        sal_Int32 size = sizeof( ***pT );
        sal_Int32 dim2 = sizeof( **pT ) / size;
        sal_Int32 dim1 = sizeof( *pT ) / dim2 / size;
        ::typelib_static_array_type_init(
            & ::com::sun::star::uno::Array< T >::s_pType, rElementType.getTypeLibType(),
            2, dim1, dim2 );
    }
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        & ::com::sun::star::uno::Array< T >::s_pType );
}

template< class T >
inline const ::com::sun::star::uno::Type & SAL_CALL getCppuArrayType3( T * pT ) SAL_THROW( () )
{
    if (! ::com::sun::star::uno::Array< T >::s_pType)
    {
        const ::com::sun::star::uno::Type & rElementType = ::getCppuType( ***pT );
        sal_Int32 size = sizeof( ****pT );
        sal_Int32 dim3 = sizeof( ***pT ) / size;
        sal_Int32 dim2 = sizeof( **pT ) / dim3 / size;
        sal_Int32 dim1 = sizeof( *pT ) / (dim2 * dim3)/ size;
        ::typelib_static_array_type_init(
            & ::com::sun::star::uno::Array< T >::s_pType, rElementType.getTypeLibType(),
            3, dim1, dim2, dim3 );
    }
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        & ::com::sun::star::uno::Array< T >::s_pType );
}

template< class T >
inline const ::com::sun::star::uno::Type & SAL_CALL getCppuArrayType4( T * pT ) SAL_THROW( () )
{
    if (! ::com::sun::star::uno::Array< T >::s_pType)
    {
        const ::com::sun::star::uno::Type & rElementType = ::getCppuType( ****pT );
        sal_Int32 size = sizeof( *****pT );
        sal_Int32 dim4 = sizeof( ****pT ) / size;
        sal_Int32 dim3 = sizeof( ***pT ) / dim4 / size;
        sal_Int32 dim2 = sizeof( **pT ) / (dim3 * dim4) / size;
        sal_Int32 dim1 = sizeof( *pT ) / (dim2 * dim3 * dim4) / size;
        ::typelib_static_array_type_init(
            & ::com::sun::star::uno::Array< T >::s_pType, rElementType.getTypeLibType(),
            4, dim1, dim2, dim3, dim4 );
    }
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        & ::com::sun::star::uno::Array< T >::s_pType );
}

template< class T >
inline const ::com::sun::star::uno::Type & SAL_CALL getCppuArrayType5( T * pT ) SAL_THROW( () )
{
    if (! ::com::sun::star::uno::Array< T >::s_pType)
    {
        const ::com::sun::star::uno::Type & rElementType = ::getCppuType( *****pT );
        sal_Int32 size = sizeof( ******pT );
        sal_Int32 dim5 = sizeof( *****pT ) / size;
        sal_Int32 dim4 = sizeof( ****pT ) / dim5 / size;
        sal_Int32 dim3 = sizeof( ***pT ) / (dim4 * dim5) / size;
        sal_Int32 dim2 = sizeof( **pT ) / (dim3 * dim4 * dim5) / size;
        sal_Int32 dim1 = sizeof( *pT ) / (dim2 * dim3 * dim4 * dim5) / size;
        ::typelib_static_array_type_init(
            & ::com::sun::star::uno::Array< T >::s_pType, rElementType.getTypeLibType(),
            5, dim1, dim2, dim3, dim4, dim5 );
    }
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        & ::com::sun::star::uno::Array< T >::s_pType );
}

template< class T >
inline const ::com::sun::star::uno::Type & SAL_CALL getCppuArrayType6( T * pT ) SAL_THROW( () )
{
    if (! ::com::sun::star::uno::Array< T >::s_pType)
    {
        const ::com::sun::star::uno::Type & rElementType = ::getCppuType( ******pT );
        sal_Int32 size = sizeof( *******pT );
        sal_Int32 dim6 = sizeof( ******pT ) / size;
        sal_Int32 dim5 = sizeof( *****pT ) / dim6 / size;
        sal_Int32 dim4 = sizeof( ****pT ) / (dim5 * dim6) / size;
        sal_Int32 dim3 = sizeof( ***pT ) / (dim4 * dim5 * dim6) / size;
        sal_Int32 dim2 = sizeof( **pT ) / (dim3 * dim4 * dim5 * dim6) / size;
        sal_Int32 dim1 = sizeof( *pT ) / (dim2 * dim3 * dim4 * dim5 * dim6) / size;
        ::typelib_static_array_type_init(
            & ::com::sun::star::uno::Array< T >::s_pType, rElementType.getTypeLibType(),
            6, dim1, dim2, dim3, dim4, dim5, dim6 );
    }
    return * reinterpret_cast< const ::com::sun::star::uno::Type * >(
        & ::com::sun::star::uno::Array< T >::s_pType );
}

template< typename T >
inline const ::com::sun::star::uno::Type & SAL_CALL getCppuType() SAL_THROW(())
{
    return getCppuType(static_cast< T * >(0));
}

template<>
inline const ::com::sun::star::uno::Type & SAL_CALL getCppuType< sal_Unicode >()
    SAL_THROW(())
{
    return getCharCppuType();
}

#endif
