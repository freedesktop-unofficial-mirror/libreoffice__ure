/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: structtypedescription.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 08:05:17 $
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

#ifndef INCLUDED_stoc_source_registry_tdprovider_structtypedescription_hxx
#define INCLUDED_stoc_source_registry_tdprovider_structtypedescription_hxx

#include "com/sun/star/reflection/XPublished.hpp"
#include "com/sun/star/reflection/XStructTypeDescription.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Sequence.hxx"
#include "cppuhelper/implbase2.hxx"
#include "rtl/ref.hxx"
#include "sal/types.h"

namespace com { namespace sun { namespace star {
    namespace container { class XHierarchicalNameAccess; }
    namespace reflection { class XCompoundTypeDescription; }
} } }
namespace rtl { class OUString; }
namespace stoc_rdbtdp { class CompoundTypeDescriptionImpl; }

namespace stoc { namespace registry_tdprovider {

class StructTypeDescription:
    public cppu::WeakImplHelper2<
        com::sun::star::reflection::XStructTypeDescription,
        com::sun::star::reflection::XPublished >
{
public:
    StructTypeDescription(
        com::sun::star::uno::Reference<
            com::sun::star::container::XHierarchicalNameAccess > const &
            manager,
        rtl::OUString const & name, rtl::OUString const & baseTypeName,
        com::sun::star::uno::Sequence< sal_Int8 > const & data, bool published);

    virtual ~StructTypeDescription();

    virtual com::sun::star::uno::TypeClass SAL_CALL getTypeClass()
        throw (com::sun::star::uno::RuntimeException);

    virtual rtl::OUString SAL_CALL getName()
        throw (com::sun::star::uno::RuntimeException);

    virtual
    com::sun::star::uno::Reference<
        com::sun::star::reflection::XTypeDescription >
    SAL_CALL getBaseType() throw (com::sun::star::uno::RuntimeException);

    virtual
    com::sun::star::uno::Sequence<
        com::sun::star::uno::Reference<
            com::sun::star::reflection::XTypeDescription > >
    SAL_CALL getMemberTypes() throw (com::sun::star::uno::RuntimeException);

    virtual com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL
    getMemberNames() throw (com::sun::star::uno::RuntimeException);

    virtual com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL
    getTypeParameters() throw (com::sun::star::uno::RuntimeException);

    virtual
    com::sun::star::uno::Sequence<
        com::sun::star::uno::Reference<
            com::sun::star::reflection::XTypeDescription > >
    SAL_CALL getTypeArguments() throw (com::sun::star::uno::RuntimeException);

    virtual sal_Bool SAL_CALL isPublished()
        throw (com::sun::star::uno::RuntimeException);

private:
    StructTypeDescription(StructTypeDescription &); // not implemented
    void operator =(StructTypeDescription); // not implemented

    com::sun::star::uno::Sequence< sal_Int8 > m_data;
    rtl::Reference< stoc_rdbtdp::CompoundTypeDescriptionImpl > m_base;
};

} }

#endif
