/*************************************************************************
 *
 *  $RCSfile: Reference.h,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: hr $ $Date: 2000-09-18 15:25:51 $
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
#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#define _COM_SUN_STAR_UNO_REFERENCE_H_

#ifndef _RTL_ALLOC_H_
#include <rtl/alloc.h>
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

class XInterface;

/** Enum defining UNO_REF_NO_ACQUIRE for setting reference without acquiring a given interface.
    <br>
*/
enum __UnoReference_NoAcquire
{
    /** This enum value can be used for creating a reference granting a given
        interface, i.e. transferring ownership to it.
        <br>
    */
    UNO_REF_NO_ACQUIRE = 0xbebeef
};

/** Base reference class holding/ acquiring an interface.<br>
    Constructors acquire an interface while the destructor releases it.
    This class serves as a base class for all template reference classes and
    has been introduced due to compiler problems with templated operators ==, =!.
    <br>
*/
class BaseReference
{
    /** the interface pointer<br>
    */
    XInterface * _pInterface;
    
protected:
    /** Default Constructor:
        Sets null reference.
        <br>
    */
    inline BaseReference();
    /** Constructor:
        Sets given interface pointer.
        <br>
        @param pInterface an interface pointer
    */
    inline BaseReference( XInterface * pInterface );
    /** Constructor:
        Sets reference to given interface pointer without acquiring it.
        <br>
        @param pInterface interface pointer
        @param dummy UNO_REF_NO_ACQUIRE to force obvious distinction to other constructors
    */
    inline BaseReference( XInterface * pInterface, __UnoReference_NoAcquire );
    /** Copy constructor:
        Copies interface reference.
        <br>
        @param rRef another reference
    */
    inline BaseReference( const BaseReference & rRef );
    /** Destructor:
        Releases interface reference.
        <br>
    */
    inline ~BaseReference();
    
public:
    // these are here to force memory de/allocation to sal lib.
    inline static void * SAL_CALL operator new( size_t nSize ) throw()
        { return ::rtl_allocateMemory( nSize ); }
    inline static void SAL_CALL operator delete( void * pMem ) throw()
        { ::rtl_freeMemory( pMem ); }
    inline static void * SAL_CALL operator new( size_t, void * pMem ) throw()
        { return pMem; }
    inline static void SAL_CALL operator delete( void *, void * ) throw()
        {}
    
    /** Sets interface pointer. An interface already set will be released.
        <br>
        @param pInterface an interface pointer
    */
    inline void SAL_CALL set( XInterface * pInterface );
    /** Clears reference, i.e. releases interface.
        Reference is null after clear() call.
        <br>
    */
    inline void	SAL_CALL clear();
    /** Gets interface pointer.
        This call does <b>not</b> acquire the interface.
        <br>
        @return <b>un</b>acquired interface pointer
    */
    inline XInterface * SAL_CALL get() const
        { return _pInterface; }
    /** Checks if reference is null.
        <br>
        @return true if reference acquires an interface, i.e. is not null
    */
    inline sal_Bool SAL_CALL is() const
        { return (_pInterface != 0); }
    /** Equality operator: compares two interfaces<br>
        Checks if both references are null or refer to the same object.
        <br>
        @param rRef another reference
        @return true if both references are null or refer to the same object, false otherwise
    */
    inline sal_Bool SAL_CALL operator == ( const BaseReference & rRef ) const;
    /** Unequality operator: compares two interfaces<br>
        Checks if both references are null or refer to the same object.
        <br>
        @param rRef another reference
        @return false if both references are null or refer to the same object, true otherwise
    */
    inline sal_Bool SAL_CALL operator != ( const BaseReference & rRef ) const
        { return (! operator == ( rRef )); }
    
    // needed for stl container operations, though this makes no sense on pointers
    inline sal_Bool SAL_CALL operator < ( const BaseReference& rRef ) const
        { return (_pInterface < rRef._pInterface); }
};

/** Enum defining UNO_QUERY and UNO_REF_NO_ACQUIRE for query interface constructor
    of reference template.
    <br>
*/
enum __UnoReference_Query
{
    /** This enum value can be used for querying interface constructor of reference template.
        <br>
    */
    UNO_REF_QUERY = 0xdb0e121e,
    UNO_QUERY = 0xdb0
};

/** Template reference class for interface type derived from BaseReference.
    A special constructor given the UNO_QUERY or UNO_REF_QUERY identifier queries interfaces
    for reference type.
    <br>
*/
template< class interface_type >
class Reference : public BaseReference
{
public:
    // these are here to force memory de/allocation to sal lib.
    static void * SAL_CALL operator new( size_t nSize ) throw()
        { return ::rtl_allocateMemory( nSize ); }
    static void SAL_CALL operator delete( void * pMem ) throw()
        { ::rtl_freeMemory( pMem ); }
    static void * SAL_CALL operator new( size_t, void * pMem ) throw()
        { return pMem; }
    static void SAL_CALL operator delete( void *, void * ) throw()
        {}
    
    /** Default Constructor:
        Sets null reference.
        <br>
    */
    inline Reference()
        : BaseReference()
        {}
    /** Copy constructor:
        Copies interface reference.
        <br>
        @param rRef another reference
    */
    inline Reference( const Reference< interface_type > & rRef )
        : BaseReference( rRef )
        {}
    /** Constructor:
        Sets given interface pointer.
        <br>
        @param pInterface an interface pointer
    */
    inline Reference( interface_type * pInterface )
        : BaseReference( pInterface )
        {}
    
    /** Constructor:
        Sets given interface pointer without acquiring it.
        <br>
        @param pInterface another reference
        @param dummy UNO_REF_NO_ACQUIRE to force obvious distinction to other constructors
    */
    inline Reference( XInterface * pInterface, __UnoReference_NoAcquire )
        : BaseReference( pInterface, UNO_REF_NO_ACQUIRE )
        {}
    
    /** Constructor:
        Queries given interface for reference interface type (<b>interface_type</b>).
        <br>
        @param rRef another reference
        @param dummy UNO_QUERY or UNO_REF_QUERY to force obvious distinction to other constructors
    */
    inline Reference( const BaseReference & rRef, __UnoReference_Query )
        : BaseReference( query( rRef ) )
        {}
    /** Constructor:
        Queries given interface for reference interface type (<b>interface_type</b>).
        <br>
        @param pInterface an interface pointer
        @param dummy UNO_QUERY to force obvious distinction to other constructors
    */
    inline Reference( XInterface * pInterface, __UnoReference_Query )
        : BaseReference( query( pInterface ) )
        {}
    
    /** Assignment operator:
        Acquires given interface pointer and sets reference.
        An interface already set will be released.
        <br>
        @param pInterface an interface pointer
        @return this reference
    */
    inline Reference< interface_type > & SAL_CALL operator = ( interface_type * pInterface );
    /** Assignment operator:
        Acquires given interface reference and sets reference.
        An interface already set will be released.
        <br>
        @param rRef an interface reference
        @return this reference
    */
    inline Reference< interface_type > & SAL_CALL operator = ( const Reference< interface_type > & rRef )
        { return operator = ( rRef.get() ); }
    
    /** Queries given interface for type <b>interface_type</b>.
        <br>
        @param pInterface interface pointer
        @return interface reference of demanded type (may be null)
    */
    inline static Reference< interface_type > SAL_CALL query( XInterface * pInterface );
    /** Queries given interface reference for type <b>interface_type</b>.
        <br>
        @param rRef interface reference
        @return interface reference of demanded type (may be null)
    */
    inline static Reference< interface_type > SAL_CALL query( const BaseReference & rRef )
        { return query( rRef.get() ); }
    
    /** Cast operatory to Reference< XInterface >:
        Reference objects are binary compatible and any interface must be derived
        from com.sun.star.uno.XInterface.
        This a useful direct cast possibility.
        <br>
    */
    inline SAL_CALL operator const Reference< XInterface > & () const
        { return * reinterpret_cast< const Reference< XInterface > * >( this ); }
    
    /** Dereference operator:
        Used to call interface methods.
        <br>
        @return <b>un</b>acquired interface pointer
    */
    interface_type * SAL_CALL operator -> () const 
        { return static_cast< interface_type * >( BaseReference::get() ); }
    
    /** Gets interface pointer.
        This call does <b>not</b> acquire the interface.
        <br>
        @return <b>un</b>acquired interface pointer
    */
    interface_type * SAL_CALL get() const 
        { return static_cast< interface_type * >( BaseReference::get() ); }
};

}     
}     
}
}

#endif
