/*************************************************************************
 *
 *  $RCSfile: implbase1.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: dbo $ $Date: 2001-05-21 09:14:50 $
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
#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#define _CPPUHELPER_IMPLBASE1_HXX_

#ifndef _CPPUHELPER_IMPLBASE_HXX_
#include <cppuhelper/implbase.hxx>
#endif

/*
#define __IFC1 Ifc1
#define __CLASS_IFC1 class Ifc1
#define __PUBLIC_IFC1 public Ifc1
__DEF_IMPLHELPER_PRE( 1 )
    __IFC_WRITEOFFSET( 1 )
__DEF_IMPLHELPER_POST( 1 )
*/

namespace cppu
{
    struct ClassData1 : public ClassDataBase
    {
        Type_Offset arType2Offset[ 1 ];
        ClassData1( sal_Int32 nClassCode ) SAL_THROW( () )
            : ClassDataBase( nClassCode )
            {}
    };
    /** This template class serves as a base class for all subsequent implementation helper classes.
        It inherits from given interfaces (template parameters) and com.sun.star.lang.XTypeProvider.
        
        Do not use this class directly, use
               ImplHelperBaseN<>,
               WeakImplHelperN<>,
               WeakAggImplHelperN<>
               WeakComponentImplHelperN<>,
               WeakAggComponentImplHelperN<>.
    */
    template< class Ifc1 >
    class SAL_NO_VTABLE ImplHelperBase1
        : public ::com::sun::star::lang::XTypeProvider
        , public Ifc1
    {
    protected:
        ClassData & SAL_CALL getClassData( ClassDataBase & s_aCD ) SAL_THROW( () )
            {
                ClassData & rCD = * static_cast< ClassData * >( &s_aCD );
                if (! rCD.bOffsetsInit)
                {
                    ::osl::MutexGuard aGuard( getImplHelperInitMutex() );
                    if (! rCD.bOffsetsInit)
                    {
                        char * pBase = (char *)this;
                        rCD.writeTypeOffset( ::getCppuType( (const ::com::sun::star::uno::Reference< Ifc1 > *)0 ),
                                             (char *)(Ifc1 *)this - pBase );
                        rCD.bOffsetsInit = sal_True;
                    }
                }
                return rCD;
            }
    };
    /** This template class inherits from ImplHelperBaseN<> and implements
        com.sun.star.uno.XInterface::queryInterface() and the com.sun.star.lang.XTypeProvider
        interface.
        
        All other virtual functions (inherited from given template parameter interfaces)
        have to be implemented by sub-classing from the templated class, e.g.
        class MyImpl : public ::cppu::ImplHelperN<> { ... };
    */
    template< class Ifc1 >
    class SAL_NO_VTABLE ImplHelper1
        : public ImplHelperBase1< Ifc1 >
    {
        static ClassData1 s_aCD;
    public:
        virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw (::com::sun::star::uno::RuntimeException)
            { return getClassData( s_aCD ).query( rType, (ImplHelperBase1< Ifc1 > *)this ); }
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes() throw (::com::sun::star::uno::RuntimeException)
            { return getClassData( s_aCD ).getTypes(); }
        virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId() throw (::com::sun::star::uno::RuntimeException)
            { return getClassData( s_aCD ).getImplementationId(); }
    };
    /** This template class inherits from ::cppu::ImplHelperBaseN<> and ::cppu::OWeakObject,
        thus delegating life-cycle to that implementation.
        Use this helper implementing an object, that can be held weakly, using the
        ::cppu::WeakReference<> template class.
        
        All other virtual functions (inherited from given template parameter interfaces)
        have to be implemented by sub-classing from the templated class, e.g.
        class MyImpl : public ::cppu::WeakImplHelperN<> { ... };
    */
    template< class Ifc1 >
    class SAL_NO_VTABLE WeakImplHelper1
        : public ::cppu::OWeakObject
        , public ImplHelperBase1< Ifc1 >
    {
        static ClassData1 s_aCD;
    public:
        virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw (::com::sun::star::uno::RuntimeException)
            {
                ::com::sun::star::uno::Any aRet( getClassData( s_aCD ).query( rType, (ImplHelperBase1< Ifc1 > *)this ) );
                return (aRet.hasValue() ? aRet : OWeakObject::queryInterface( rType ));
            }
        virtual void SAL_CALL acquire() throw ()
            { OWeakObject::acquire(); }
        virtual void SAL_CALL release() throw ()
            { OWeakObject::release(); }
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes() throw (::com::sun::star::uno::RuntimeException)
            { return getClassData( s_aCD ).getTypes(); }
        virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId() throw (::com::sun::star::uno::RuntimeException)
            { return getClassData( s_aCD ).getImplementationId(); }
    };
    /** This template class inherits from ::cppu::ImplHelperBaseN<> and ::cppu::OWeakAggObject,
        thus delegating life-cycle to that implementation.
        Use this helper implementing an object, that can be held weakly using the
        ::cppu::WeakReference<> template class and can be aggregated by other objects.
        
        All other virtual functions (inherited from given template parameter interfaces)
        have to be implemented by sub-classing from the templated class, e.g.
        class MyImpl : public ::cppu::WeakAggImplHelperN<> { ... };
    */
    template< class Ifc1 >
    class SAL_NO_VTABLE WeakAggImplHelper1
        : public ::cppu::OWeakAggObject
        , public ImplHelperBase1< Ifc1 >
    {
        static ClassData1 s_aCD;
    public:
        virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw (::com::sun::star::uno::RuntimeException)
            { return OWeakAggObject::queryInterface( rType ); }
        virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type & rType ) throw (::com::sun::star::uno::RuntimeException)
            {
                ::com::sun::star::uno::Any aRet( getClassData( s_aCD ).query( rType, (ImplHelperBase1< Ifc1 > *)this ) );
                return (aRet.hasValue() ? aRet : OWeakAggObject::queryAggregation( rType ));
            }
        virtual void SAL_CALL acquire() throw ()
            { OWeakAggObject::acquire(); }
        virtual void SAL_CALL release() throw ()
            { OWeakAggObject::release(); }
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes() throw (::com::sun::star::uno::RuntimeException)
            { return getClassData( s_aCD ).getTypes(); }
        virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId() throw (::com::sun::star::uno::RuntimeException)
            { return getClassData( s_aCD ).getImplementationId(); }
    };

#ifndef MACOSX
    template< class Ifc1 >
    ClassData1 ImplHelper1< Ifc1 >::s_aCD = ClassData1( 0 );
    template< class Ifc1 >
    ClassData1 WeakImplHelper1< Ifc1 >::s_aCD = ClassData1( 1 );
    template< class Ifc1 >
    ClassData1 WeakAggImplHelper1< Ifc1 >::s_aCD = ClassData1( 2 );
#endif

}

#endif
