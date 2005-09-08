/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: servicefactory.hxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 09:20:52 $
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
#ifndef _CPPUHELPER_SERVICEFACTORY_HXX_
#define _CPPUHELPER_SERVICEFACTORY_HXX_

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>


namespace cppu
{

/** Deprecated.  Use cppuhelper/bootstrap.hxx functions instead.
    
    This bootstraps an initial service factory working on a registry. If the first or both 
    parameters contain a value then the service factory is initialized with a simple registry
    or a nested registry. Otherwise the service factory must be initialized later with a valid
    registry.
    
    @param rWriteRegistryFile
    file name of the simple registry or the first registry file of
    the nested registry which will be opened with read/write rights. This
    file will be created if necessary.
    @param rReadRegistryFile
    file name of the second registry file of the nested registry 
    which will be opened with readonly rights.
    @param bReadOnly
    flag which specify that the first registry file will be opened with 
    readonly rights. Default is FALSE. If this flag is used the registry 
    will not be created if not exist.
    @param rBootstrapPath
    specifies a path where the bootstrap components could be find. If this
    parameter is an empty string the compoents will be searched in the system
    path.
    @deprecated
*/
::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > SAL_CALL
createRegistryServiceFactory(
    const ::rtl::OUString & rWriteRegistryFile, 
    const ::rtl::OUString & rReadRegistryFile,
    sal_Bool bReadOnly = sal_False,
    const ::rtl::OUString & rBootstrapPath = ::rtl::OUString() )
    SAL_THROW( (::com::sun::star::uno::Exception) );


/** Deprecated.  Use cppuhelper/bootstrap.hxx functions instead.
    
    This bootstraps an initial service factory working on a registry file.
    
    @param rRegistryFile
    file name of the registry to use/ create; if this is an empty
    string, the default registry is used instead
    @param bReadOnly
    flag which specify that the registry file will be opened with 
    readonly rights. Default is FALSE. If this flag is used the registry 
    will not be created if not exist.
    @param rBootstrapPath
    specifies a path where the bootstrap components could be find. If this
    parameter is an empty string the compoents will be searched in the system
    path.
    @deprecated
*/
inline ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > SAL_CALL
createRegistryServiceFactory(
    const ::rtl::OUString & rRegistryFile, 
    sal_Bool bReadOnly = sal_False,
    const ::rtl::OUString & rBootstrapPath = ::rtl::OUString() )
    SAL_THROW( (::com::sun::star::uno::Exception) )
{
    return ::cppu::createRegistryServiceFactory(
        rRegistryFile, ::rtl::OUString(), bReadOnly, rBootstrapPath );
}

/** Deprecated.  Use cppuhelper/bootstrap.hxx functions instead.
    
    This bootstraps a service factory without initialize a registry.

    @param rBootstrapPath
    specifies a path where the bootstrap components could be find. If this
    parameter is an empty string the compoents will be searched in the system
    path.
    @deprecated
*/
inline ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > SAL_CALL
createServiceFactory(
    const ::rtl::OUString & rBootstrapPath = ::rtl::OUString() )
    SAL_THROW( (::com::sun::star::uno::Exception) )
{
    return ::cppu::createRegistryServiceFactory(
        ::rtl::OUString(), ::rtl::OUString(), sal_False, rBootstrapPath );
}

} // end namespace cppu

#endif

