/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: rdbtdp_tdenumeration.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 08:04:38 $
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

#ifndef _STOC_RDBTDP_TDENUMERATION_HXX
#define _STOC_RDBTDP_TDENUMERATION_HXX

#include <list>

#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif
#ifndef _RTL_REF_HXX_
#include <rtl/ref.hxx>
#endif
#ifndef _REGISTRY_REFLTYPE_HXX_
#include <registry/refltype.hxx>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XHIERARCHICALNAMEACCESS_HPP_
#include <com/sun/star/container/XHierarchicalNameAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_REFLECTION_INVALIDTYPENAMEEXCEPTION_HPP_
#include <com/sun/star/reflection/InvalidTypeNameException.hpp>
#endif
#ifndef _COM_SUN_STAR_REFLECTION_NOSUCHTYPENAMEEXCEPTION_HPP_
#include <com/sun/star/reflection/NoSuchTypeNameException.hpp>
#endif
#ifndef _COM_SUN_STAR_REFLECTION_TYPEDESCRIPTIONSEARCHDEPTH_HPP_
#include <com/sun/star/reflection/TypeDescriptionSearchDepth.hpp>
#endif
#ifndef _COM_SUN_STAR_REFLECTION_XTYPEDESCRIPTIONENUMERATION_HPP_
#include <com/sun/star/reflection/XTypeDescriptionEnumeration.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_TYPECLASS_HPP_
#include <com/sun/star/uno/TypeClass.hpp>
#endif
#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif

#ifndef _STOC_RDBTDP_BASE_HXX
#include "base.hxx"
#endif

namespace stoc_rdbtdp
{

typedef ::std::list< ::com::sun::star::uno::Reference<
    ::com::sun::star::reflection::XTypeDescription > > TypeDescriptionList;

class TypeDescriptionEnumerationImpl
    : public cppu::WeakImplHelper1<
                com::sun::star::reflection::XTypeDescriptionEnumeration >
{
public:
    static rtl::Reference< TypeDescriptionEnumerationImpl > createInstance(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::container::XHierarchicalNameAccess > & xTDMgr,
        const rtl::OUString & rModuleName,
        const ::com::sun::star::uno::Sequence<
            ::com::sun::star::uno::TypeClass > & rTypes,
        ::com::sun::star::reflection::TypeDescriptionSearchDepth eDepth,
        const RegistryKeyList & rBaseKeys )
            throw ( ::com::sun::star::reflection::NoSuchTypeNameException,
                    ::com::sun::star::reflection::InvalidTypeNameException,
                    ::com::sun::star::uno::RuntimeException );

    virtual ~TypeDescriptionEnumerationImpl();

    // XEnumeration (base of XTypeDescriptionEnumeration)
    virtual sal_Bool SAL_CALL hasMoreElements()
        throw ( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Any SAL_CALL nextElement()
        throw ( ::com::sun::star::container::NoSuchElementException,
                ::com::sun::star::lang::WrappedTargetException,
                ::com::sun::star::uno::RuntimeException );

    // XTypeDescriptionEnumeration
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::reflection::XTypeDescription > SAL_CALL
    nextTypeDescription()
        throw ( ::com::sun::star::container::NoSuchElementException,
                ::com::sun::star::uno::RuntimeException );

private:
    // Note: keys must be open (XRegistryKey->openKey(...)).
    TypeDescriptionEnumerationImpl(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::container::XHierarchicalNameAccess > & xTDMgr,
        const RegistryKeyList & rModuleKeys,
        const ::com::sun::star::uno::Sequence<
            ::com::sun::star::uno::TypeClass > & rTypes,
        ::com::sun::star::reflection::TypeDescriptionSearchDepth eDepth );

    static bool match( ::RTTypeClass eType1,
                       ::com::sun::star::uno::TypeClass eType2 );
    bool queryMore();
    ::com::sun::star::uno::Reference<
        ::com::sun::star::reflection::XTypeDescription > queryNext();

    // members
    osl::Mutex m_aMutex;
    RegistryKeyList     m_aModuleKeys;
    RegistryKeyList     m_aCurrentModuleSubKeys;
    TypeDescriptionList m_aTypeDescs;
    ::com::sun::star::uno::Sequence<
        ::com::sun::star::uno::TypeClass > m_aTypes;
    ::com::sun::star::reflection::TypeDescriptionSearchDepth m_eDepth;
    ::com::sun::star::uno::Reference<
        ::com::sun::star::container::XHierarchicalNameAccess > m_xTDMgr;
};

} // namespace stoc_rdbtdp

#endif /* _STOC_RDBTDP_TDENUMERATION_HXX */

