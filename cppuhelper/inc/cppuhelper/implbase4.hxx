/*************************************************************************
 *
 *  $RCSfile: implbase4.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: dbo $ $Date: 2001-05-14 11:58:06 $
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
#ifndef _CPPUHELPER_IMPLBASE4_HXX_
#define _CPPUHELPER_IMPLBASE4_HXX_

#ifndef _CPPUHELPER_IMPLBASE_HXX_
#include <cppuhelper/implbase.hxx>
#endif

/*
#define __IFC4 Ifc1, Ifc2, Ifc3, Ifc4
#define __CLASS_IFC4 class Ifc1, class Ifc2, class Ifc3, class Ifc4
#define __PUBLIC_IFC4 public Ifc1, public Ifc2, public Ifc3, public Ifc4
__DEF_IMPLHELPER_PRE( 4 )
    __IFC_WRITEOFFSET( 1 ) __IFC_WRITEOFFSET( 2 ) __IFC_WRITEOFFSET( 3 ) __IFC_WRITEOFFSET( 4 )
__DEF_IMPLHELPER_POST( 4 )
*/

namespace cppu
{
    struct ClassData4 : public ClassDataBase
    {
        Type_Offset arType2Offset[ 4 ];
        ClassData4( sal_Int32 nClassCode ) SAL_THROW( () )
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
    template< class Ifc1, class Ifc2, class Ifc3, class Ifc4 >
    class ImplHelperBase4
        : public ::com::sun::star::lang::XTypeProvider
        , public Ifc1, public Ifc2, public Ifc3, public Ifc4
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
                        rCD.writeTypeOffset( ::getCppuType( (const ::com::sun::star::uno::Reference< Ifc2 > *)0 ),
                                             (char *)(Ifc2 *)this - pBase );
                        rCD.writeTypeOffset( ::getCppuType( (const ::com::sun::star::uno::Reference< Ifc3 > *)0 ),
                                             (char *)(Ifc3 *)this - pBase );
                        rCD.writeTypeOffset( ::getCppuType( (const ::com::sun::star::uno::Reference< Ifc4 > *)0 ),
                                             (char *)(Ifc4 *)this - pBase );
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
    template< class Ifc1, class Ifc2, class Ifc3, class Ifc4 >
    class ImplHelper4
        : public ImplHelperBase4< Ifc1, Ifc2, Ifc3, Ifc4 >
    {
        static ClassData4 s_aCD;
    public:
        virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw (::com::sun::star::uno::RuntimeException)
            { return getClassData( s_aCD ).query( rType, (ImplHelperBase4< Ifc1, Ifc2, Ifc3, Ifc4 > *)this ); }
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
    template< class Ifc1, class Ifc2, class Ifc3, class Ifc4 >
    class WeakImplHelper4
        : public ::cppu::OWeakObject
        , public ImplHelperBase4< Ifc1, Ifc2, Ifc3, Ifc4 >
    {
        static ClassData4 s_aCD;
    public:
        virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw (::com::sun::star::uno::RuntimeException)
            {
                ::com::sun::star::uno::Any aRet( getClassData( s_aCD ).query( rType, (ImplHelperBase4< Ifc1, Ifc2, Ifc3, Ifc4 > *)this ) );
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
    template< class Ifc1, class Ifc2, class Ifc3, class Ifc4 >
    class WeakAggImplHelper4
        : public ::cppu::OWeakAggObject
        , public ImplHelperBase4< Ifc1, Ifc2, Ifc3, Ifc4 >
    {
        static ClassData4 s_aCD;
    public:
        virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw (::com::sun::star::uno::RuntimeException)
            { return OWeakAggObject::queryInterface( rType ); }
        virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type & rType ) throw (::com::sun::star::uno::RuntimeException)
            {
                ::com::sun::star::uno::Any aRet( getClassData( s_aCD ).query( rType, (ImplHelperBase4< Ifc1, Ifc2, Ifc3, Ifc4 > *)this ) );
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
    template< class Ifc1, class Ifc2, class Ifc3, class Ifc4 >
    ClassData4 ImplHelper4< Ifc1, Ifc2, Ifc3, Ifc4 >::s_aCD = ClassData4( 0 );
    template< class Ifc1, class Ifc2, class Ifc3, class Ifc4 >
    ClassData4 WeakImplHelper4< Ifc1, Ifc2, Ifc3, Ifc4 >::s_aCD = ClassData4( 1 );
    template< class Ifc1, class Ifc2, class Ifc3, class Ifc4 >
    ClassData4 WeakAggImplHelper4< Ifc1, Ifc2, Ifc3, Ifc4 >::s_aCD = ClassData4( 2 );
#endif

}

#endif
