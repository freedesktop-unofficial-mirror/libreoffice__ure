/*************************************************************************
 *
 *  $RCSfile: Reference.hxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: dbo $ $Date: 2002-08-19 07:18:44 $
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
#ifndef _COM_SUN_STAR_UNO_REFERENCE_HXX_
#define _COM_SUN_STAR_UNO_REFERENCE_HXX_

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif
#ifndef _COM_SUN_STAR_UNO_XINTERFACE_HDL_
#include <com/sun/star/uno/XInterface.hdl>
#endif
#ifndef _COM_SUN_STAR_UNO_GENFUNC_HXX_
#include <com/sun/star/uno/genfunc.hxx>
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
inline XInterface * BaseReference::__query(
    XInterface * pInterface, const Type & rType )
    SAL_THROW( (RuntimeException) )
{
    if (pInterface)
    {
        Any aRet( pInterface->queryInterface( rType ) );
        if (typelib_TypeClass_INTERFACE == aRet.pType->eTypeClass)
        {
            XInterface * pRet = reinterpret_cast< XInterface * >( aRet.pReserved );
            aRet.pReserved = 0;
            return pRet;
        }
    }
    return 0;
}
//__________________________________________________________________________________________________
template< class interface_type >
inline interface_type * Reference< interface_type >::__query(
    XInterface * pInterface ) SAL_THROW( (RuntimeException) )
{
    return static_cast< interface_type * >(
        BaseReference::__query(
            pInterface, ::getCppuType( (const Reference< interface_type > *)0 ) ) );
}
#ifndef EXCEPTIONS_OFF
extern "C" rtl_uString * SAL_CALL __cppu_unsatisfied_query_msg(
    typelib_TypeDescriptionReference * pType )
    SAL_THROW_EXTERN_C();
//__________________________________________________________________________________________________
inline XInterface * BaseReference::__query_throw(
    XInterface * pInterface, const Type & rType )
    SAL_THROW( (RuntimeException) )
{
    XInterface * pQueried = __query( pInterface, rType );
    if (pQueried)
        return pQueried;
    throw RuntimeException(
        ::rtl::OUString( __cppu_unsatisfied_query_msg( rType.getTypeLibType() ), SAL_NO_ACQUIRE ),
        Reference< XInterface >( pInterface ) );
}
//__________________________________________________________________________________________________
template< class interface_type >
inline interface_type * Reference< interface_type >::__query_throw(
    XInterface * pInterface ) SAL_THROW( (RuntimeException) )
{
    return static_cast< interface_type * >(
        BaseReference::__query_throw(
            pInterface, ::getCppuType( (const Reference< interface_type > *)0 ) ) );
}
#endif

//__________________________________________________________________________________________________
template< class interface_type >
inline Reference< interface_type >::~Reference() SAL_THROW( () )
{
    if (_pInterface)
        _pInterface->release();
}
//__________________________________________________________________________________________________
template< class interface_type >
inline Reference< interface_type >::Reference() SAL_THROW( () )
{
    _pInterface = 0;
}
//__________________________________________________________________________________________________
template< class interface_type >
inline Reference< interface_type >::Reference( const Reference< interface_type > & rRef ) SAL_THROW( () )
{
    _pInterface = rRef._pInterface;
    if (_pInterface)
        _pInterface->acquire();
}
//__________________________________________________________________________________________________
template< class interface_type >
inline Reference< interface_type >::Reference( interface_type * pInterface ) SAL_THROW( () )
{
    _pInterface = pInterface;
    if (_pInterface)
        _pInterface->acquire();
}
//__________________________________________________________________________________________________
template< class interface_type >
inline Reference< interface_type >::Reference( interface_type * pInterface, __sal_NoAcquire ) SAL_THROW( () )
{
    _pInterface = pInterface;
}
//__________________________________________________________________________________________________
template< class interface_type >
inline Reference< interface_type >::Reference( interface_type * pInterface, __UnoReference_NoAcquire ) SAL_THROW( () )
{
    _pInterface = pInterface;
}
//__________________________________________________________________________________________________
template< class interface_type >
inline Reference< interface_type >::Reference( const BaseReference & rRef, __UnoReference_Query ) SAL_THROW( (RuntimeException) )
{
    _pInterface = __query( rRef.get() );
}
//__________________________________________________________________________________________________
template< class interface_type >
inline Reference< interface_type >::Reference( XInterface * pInterface, __UnoReference_Query ) SAL_THROW( (RuntimeException) )
{
    _pInterface = __query( pInterface );
}
//__________________________________________________________________________________________________
template< class interface_type >
inline Reference< interface_type >::Reference( const Any & rAny, __UnoReference_Query ) SAL_THROW( (RuntimeException) )
{
    _pInterface = (typelib_TypeClass_INTERFACE == rAny.pType->eTypeClass
                   ? __query( reinterpret_cast< XInterface * >( rAny.pReserved ) ) : 0);
}
#ifndef EXCEPTIONS_OFF
//__________________________________________________________________________________________________
template< class interface_type >
inline Reference< interface_type >::Reference( const BaseReference & rRef, __UnoReference_QueryThrow ) SAL_THROW( (RuntimeException) )
{
    _pInterface = __query_throw( rRef.get() );
}
//__________________________________________________________________________________________________
template< class interface_type >
inline Reference< interface_type >::Reference( XInterface * pInterface, __UnoReference_QueryThrow ) SAL_THROW( (RuntimeException) )
{
    _pInterface = __query_throw( pInterface );
}
//__________________________________________________________________________________________________
template< class interface_type >
inline Reference< interface_type >::Reference( const Any & rAny, __UnoReference_QueryThrow ) SAL_THROW( (RuntimeException) )
{
    _pInterface = __query_throw( typelib_TypeClass_INTERFACE == rAny.pType->eTypeClass
                                 ? reinterpret_cast< XInterface * >( rAny.pReserved ) : 0 );
}
#endif

//__________________________________________________________________________________________________
template< class interface_type >
inline void Reference< interface_type >::clear() SAL_THROW( () )
{
    if (_pInterface)
    {
        _pInterface->release();
        _pInterface = 0;
    }
}
//__________________________________________________________________________________________________
template< class interface_type >
inline sal_Bool Reference< interface_type >::set(
    interface_type * pInterface ) SAL_THROW( () )
{
    if (pInterface)
        pInterface->acquire();
    if (_pInterface)
        _pInterface->release();
    _pInterface = pInterface;
    return (0 != pInterface);
}
//__________________________________________________________________________________________________
template< class interface_type >
inline sal_Bool Reference< interface_type >::set(
    interface_type * pInterface, __sal_NoAcquire ) SAL_THROW( () )
{
    if (_pInterface)
        _pInterface->release();
    _pInterface = pInterface;
    return (0 != pInterface);
}
//__________________________________________________________________________________________________
template< class interface_type >
inline sal_Bool Reference< interface_type >::set(
    interface_type * pInterface, __UnoReference_NoAcquire ) SAL_THROW( () )
{
    return set( pInterface, SAL_NO_ACQUIRE );
}

//__________________________________________________________________________________________________
template< class interface_type >
inline sal_Bool Reference< interface_type >::set(
    const Reference< interface_type > & rRef ) SAL_THROW( () )
{
    return set( static_cast< interface_type * >( rRef._pInterface ) );
}
//__________________________________________________________________________________________________
template< class interface_type >
inline sal_Bool Reference< interface_type >::set(
    XInterface * pInterface, __UnoReference_Query ) SAL_THROW( (RuntimeException) )
{
    return set( __query( pInterface ), SAL_NO_ACQUIRE );
}
//__________________________________________________________________________________________________
template< class interface_type >
inline sal_Bool Reference< interface_type >::set(
    const BaseReference & rRef, __UnoReference_Query ) SAL_THROW( (RuntimeException) )
{
    return set( __query( rRef.get() ), SAL_NO_ACQUIRE );
}
#ifndef EXCEPTIONS_OFF
//__________________________________________________________________________________________________
template< class interface_type >
inline void Reference< interface_type >::set(
    XInterface * pInterface, __UnoReference_QueryThrow ) SAL_THROW( (RuntimeException) )
{
    set( __query_throw( pInterface ), SAL_NO_ACQUIRE );
}
//__________________________________________________________________________________________________
template< class interface_type >
inline void Reference< interface_type >::set(
    const BaseReference & rRef, __UnoReference_QueryThrow ) SAL_THROW( (RuntimeException) )
{
    set( __query_throw( rRef.get() ), SAL_NO_ACQUIRE );
}
#endif

//__________________________________________________________________________________________________
template< class interface_type >
inline Reference< interface_type > & Reference< interface_type >::operator = (
    interface_type * pInterface ) SAL_THROW( () )
{
    set( pInterface );
    return *this;
}
//__________________________________________________________________________________________________
template< class interface_type >
inline Reference< interface_type > & Reference< interface_type >::operator = (
    const Reference< interface_type > & rRef ) SAL_THROW( () )
{
    set( static_cast< interface_type * >( rRef._pInterface ) );
    return *this;
}

//__________________________________________________________________________________________________
template< class interface_type >
inline Reference< interface_type > Reference< interface_type >::query(
    const BaseReference & rRef ) SAL_THROW( (RuntimeException) )
{
    return Reference< interface_type >( __query( rRef.get() ), SAL_NO_ACQUIRE );
}
//__________________________________________________________________________________________________
template< class interface_type >
inline Reference< interface_type > Reference< interface_type >::query(
    XInterface * pInterface ) SAL_THROW( (RuntimeException) )
{
    return Reference< interface_type >( __query( pInterface ), SAL_NO_ACQUIRE );
}

//##################################################################################################

//__________________________________________________________________________________________________
inline sal_Bool BaseReference::operator == ( XInterface * pInterface ) const SAL_THROW( () )
{
    if (_pInterface == pInterface)
        return sal_True;
#ifndef EXCEPTIONS_OFF
    try
    {
#endif
        // only the query to XInterface must return the same pointer if they belong to same objects
        Reference< XInterface > x1( _pInterface, UNO_QUERY );
        Reference< XInterface > x2( pInterface, UNO_QUERY );
        return (x1._pInterface == x2._pInterface);
#ifndef EXCEPTIONS_OFF
    }
    catch (RuntimeException &)
    {
        return sal_False;
    }
#endif
}
//__________________________________________________________________________________________________
inline sal_Bool BaseReference::operator != ( XInterface * pInterface ) const SAL_THROW( () )
{
    return (! operator == ( pInterface ));
}
//__________________________________________________________________________________________________
inline sal_Bool BaseReference::operator == ( const BaseReference & rRef ) const SAL_THROW( () )
{
    return operator == ( rRef._pInterface );
}
//__________________________________________________________________________________________________
inline sal_Bool BaseReference::operator != ( const BaseReference & rRef ) const SAL_THROW( () )
{
    return (! operator == ( rRef._pInterface ));
}

}
}
}
}

#endif
