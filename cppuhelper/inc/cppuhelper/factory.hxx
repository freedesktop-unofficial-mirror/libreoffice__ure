/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: factory.hxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-26 16:45:15 $
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
#ifndef _CPPUHELPER_FACTORY_HXX_
#define _CPPUHELPER_FACTORY_HXX_

#ifndef _RTL_STRING_HXX_
#include <rtl/ustring.hxx>
#endif

#ifndef _UNO_DISPATCHER_H_
#include <uno/dispatcher.h>
#endif

#ifndef _RTL_UNLOAD_H_
#include <rtl/unload.h>
#endif

#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/lang/XSingleComponentFactory.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/registry/XRegistryKey.hpp>

//##################################################################################################

#define COMPONENT_GETENV			"component_getImplementationEnvironment"
#define COMPONENT_GETENVEXT         "component_getImplementationEnvironmentExt"
#define COMPONENT_GETDESCRIPTION	"component_getDescription"
#define COMPONENT_WRITEINFO			"component_writeInfo"
#define COMPONENT_GETFACTORY		"component_getFactory"

typedef struct _uno_Environment uno_Environment;

/** Function pointer declaration.
    Function determines the environment of the component implementation, i.e. which compiler
    compiled it. If the environment is NOT session specific (needs no additional context),
    then this function should return the environment type name and leave ppEnv (to 0).
    
    @paramppEnvTypeName environment type name; string must be constant
    @param ppEnv function returns its environment if the environment is session specific,
                 i.e. has special context
*/
typedef void (SAL_CALL * component_getImplementationEnvironmentFunc)(
    const sal_Char ** ppEnvTypeName, uno_Environment ** ppEnv );

/** Function pointer declaration.
    Function determines the environment of the component implementation, i.e. the compiler.
    If the environment is NOT session specific (needs no additional context),
    then this function should return the environment type name and leave ppEnv (to 0).

    @param ppEnvTypeName environment type name; string must be a constant
    @param ppEnv         function returns an environment if the environment is session specific,
                         i.e. has special context
    @param pImplName
*/
typedef void (SAL_CALL * component_getImplementationEnvironmentExtFunc)(
    sal_Char        const ** ppEnvTypeName,
    uno_Environment       ** ppEnv,
    sal_Char        const  * pImplName,
    uno_Environment        * pTargetEnv
);

/** Function pointer declaration.
    Function retrieves a component description.
    
    @return an XML formatted string containing a short component description
    @deprecated
*/
typedef const sal_Char * (SAL_CALL * component_getDescriptionFunc)(void);

/** Function pointer declaration.
    Function writes component registry info, at least writing the supported service names.
    
    @param pServiceManager
    a service manager (the type is an XMultiServiceFactory that can be used
    by the environment returned by component_getImplementationEnvironment)
    @param pRegistryKey a registry key
    (the type is XRegistryKey that can be used by the environment
    returned by component_getImplementationEnvironment)
    @return true if everything went fine
*/
typedef sal_Bool (SAL_CALL * component_writeInfoFunc)(
    void * pServiceManager, void * pRegistryKey );

/** Function pointer declaration.
    Retrieves a factory to create component instances.

   @param pImplName
   desired implementation name
   @param pServiceManager
   a service manager (the type is XMultiServiceFactory that can be used by the environment
   returned by component_getImplementationEnvironment)
   @param pRegistryKey
   a registry key (the type is XRegistryKey that can be used by the environment
   returned by component_getImplementationEnvironment)
   @return acquired component factory
   (the type is lang::XSingleComponentFactory or lang::XSingleServiceFactory to be used by the
   environment returned by component_getImplementationEnvironment)
*/
typedef void * (SAL_CALL * component_getFactoryFunc)(
    const sal_Char * pImplName, void * pServiceManager, void * pRegistryKey );

//##################################################################################################

namespace cppu
{     

/** Function pointer declaration.
    Function creates component instance passing the component context to be used.

    @param xContext component context to be used
    @return component instance
*/
typedef ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >(
    SAL_CALL * ComponentFactoryFunc)(
        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > const & xContext )
    SAL_THROW( (::com::sun::star::uno::Exception) );

/** Creates a single component factory supporting the XSingleComponentFactory interface.
    
    @param fptr function pointer for instanciating the object
    @param rImplementationName implementation name of service
    @param rServiceNames supported services
    @param pModCount for future extension (library unloading concept).
*/
::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleComponentFactory >
SAL_CALL createSingleComponentFactory(
    ComponentFactoryFunc fptr,
    ::rtl::OUString const & rImplementationName,
    ::com::sun::star::uno::Sequence< ::rtl::OUString > const & rServiceNames,
    rtl_ModuleCount * pModCount = 0 )
    SAL_THROW( () );

/** Deprecated.  The type of the instanciate function used as argument of the create*Fcatory functions.
    
    @see createSingleFactory
    @see createOneInstanceFactory
    @deprecated
*/
typedef ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >(SAL_CALL * ComponentInstantiation)(
    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & rServiceManager );

/** Deprecated.  Creates a single service factory.
    
    @param rServiceManager		the service manager used by the implementation.
    @param rImplementationName	the implementation name. An empty string is possible.
    @param ComponentInstantiation the function pointer to create an object.
    @param rServiceNames			the service supported by the implementation.
    @param pModCount             for future extension (library unloading concept).
    @return a factory that support the interfaces XServiceProvider, XServiceInfo
    XSingleServiceFactory and XComponent.
    
    @see createOneInstanceFactory
    @deprecated
*/
::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleServiceFactory > SAL_CALL
createSingleFactory(
    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & rServiceManager,
    const ::rtl::OUString & rImplementationName,
    ComponentInstantiation pCreateFunction,
    const ::com::sun::star::uno::Sequence< ::rtl::OUString > & rServiceNames,
    rtl_ModuleCount * pModCount = 0  )
    SAL_THROW( () );

/** Deprecated.  Creates a factory wrapping another one.
    This means the methods of the interfaces XServiceProvider, XServiceInfo and
    XSingleServiceFactory are forwarded.
    @attention
    The XComponent interface is not supported!
    
    @param rServiceManager		the service manager used by the implementation.
    @param xSingleServiceFactory	the wrapped service factory.
    @return a factory that support the interfaces XServiceProvider, XServiceInfo
    XSingleServiceFactory.
    
    @see createSingleFactory
    @deprecated
*/
::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleServiceFactory > SAL_CALL
createFactoryProxy(
    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & rServiceManager,
    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleServiceFactory > & rFactory )
    SAL_THROW( () );

/** Deprecated.  Creates a single service factory which holds the instance created only once.
    
    @param rServiceManager		the service manager used by the implementation.
    @param rImplementationName	the implementation name. An empty string is possible.
    @param ComponentInstantiation the function pointer to create an object.
    @param rServiceNames			the service supported by the implementation.
    @param pModCount             for future extension (library unloading concept).
    @return a factory that support the interfaces XServiceProvider, XServiceInfo
    XSingleServiceFactory and XComponent.
    
    @see createSingleFactory
    @deprecated
*/
::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleServiceFactory > SAL_CALL
createOneInstanceFactory(
    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & rServiceManager,
    const ::rtl::OUString & rComponentName, 
    ComponentInstantiation pCreateFunction,
    const ::com::sun::star::uno::Sequence< ::rtl::OUString > & rServiceNames,
    rtl_ModuleCount * pModCount = 0  )
    SAL_THROW( () );

/** Deprecated.  Creates a single service factory based on a registry.
    
    @param rServiceManager		the service manager used by the implementation.
    @param rImplementationName	the implementation name. An empty string is possible.
    @param rImplementationKey	the registry key of the implementation section.
    @return a factory that support the interfaces XServiceProvider, XServiceInfo
    XSingleServiceFactory and XComponent.
    @deprecated
*/
::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleServiceFactory > SAL_CALL createSingleRegistryFactory(
    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & rServiceManager,
    const ::rtl::OUString & rImplementationName, 
    const ::com::sun::star::uno::Reference< ::com::sun::star::registry::XRegistryKey > & rImplementationKey )
    SAL_THROW( () );

/** Deprecated.  Creates a single service factory which holds the instance created only once
    based on a registry.
    
    @param rServiceManager		the service manager used by the implementation.
    @param rImplementationName	the implementation name. An empty string is possible.
    @param rImplementationKey	the registry key of the implementation section.
    @return a factory that support the interfaces XServiceProvider, XServiceInfo
    XSingleServiceFactory and XComponent.
    
    @see createSingleRegistryFactory
    @deprecated
*/
::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleServiceFactory > SAL_CALL createOneInstanceRegistryFactory(
    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & rServiceManager,
    const ::rtl::OUString & rComponentName, 
    const ::com::sun::star::uno::Reference< ::com::sun::star::registry::XRegistryKey > & rImplementationKey )
    SAL_THROW( () );

}     

#endif
