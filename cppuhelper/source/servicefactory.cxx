/*************************************************************************
 *
 *  $RCSfile: servicefactory.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: jbu $ $Date: 2000-10-19 14:56:08 $
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

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _OSL_MODULE_H_
#include <osl/module.h>
#endif
#ifndef _RTL_STRING_HXX_
#include <rtl/string.hxx>
#endif
#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

#ifndef _UNO_ENVIRONMENT_H_
#include <uno/environment.h>
#endif
#ifndef _UNO_MAPPING_HXX_
#include <uno/mapping.hxx>
#endif
#ifndef _CPPUHELPER_FACTORY_HXX_
#include <cppuhelper/factory.hxx>
#endif

#include <com/sun/star/container/XSet.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/registry/XSimpleRegistry.hpp>
#include <com/sun/star/registry/XImplementationRegistration.hpp>

using namespace rtl;
using namespace osl;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::registry;
using namespace com::sun::star::container;


namespace cppu
{

//==================================================================================================
static Reference< XSingleServiceFactory > loadLibComponentFactory(
    const OUString & rLibName, const OUString & rImplName, const OUString & rBootstrapPath,
    const Reference< XMultiServiceFactory > & xSF, const Reference< XRegistryKey > & xKey )
{
    Reference< XSingleServiceFactory > xRet;
    sal_uInt32 bufLen = 32;
    
    if ( rBootstrapPath.getLength() > 0 )
    {
        bufLen += rBootstrapPath.getLength() + 1;
    }
    
    OUStringBuffer aLibNameBuf( bufLen );
    if ( bufLen > 32 )
    {
        aLibNameBuf.append( rBootstrapPath );
        aLibNameBuf.appendAscii( RTL_CONSTASCII_STRINGPARAM("/") );
    }

#ifdef SAL_UNX
    aLibNameBuf.appendAscii( RTL_CONSTASCII_STRINGPARAM("lib") );
    aLibNameBuf.append( rLibName );
    aLibNameBuf.appendAscii( RTL_CONSTASCII_STRINGPARAM(".so") );
#else
    aLibNameBuf.append( rLibName );
    aLibNameBuf.appendAscii( RTL_CONSTASCII_STRINGPARAM(".dll") );
#endif
    OUString aLibName( aLibNameBuf.makeStringAndClear() );
    oslModule lib = osl_loadModule( aLibName.pData, SAL_LOADMODULE_LAZY | SAL_LOADMODULE_GLOBAL );
    
    if (lib)
    {
        void * pSym;
        
        // ========================= LATEST VERSION =========================
        OUString aGetEnvName( RTL_CONSTASCII_USTRINGPARAM(COMPONENT_GETENV) );
        if (pSym = osl_getSymbol( lib, aGetEnvName.pData ))
        {
            uno_Environment * pCurrentEnv = 0;
            uno_Environment * pEnv = 0;
            const sal_Char * pEnvTypeName = 0;
            (*((component_getImplementationEnvironmentFunc)pSym))( &pEnvTypeName, &pEnv );
            
            sal_Bool bNeedsMapping =
                (pEnv || 0 != rtl_str_compare( pEnvTypeName, CPPU_CURRENT_LANGUAGE_BINDING_NAME ));
            
            OUString aEnvTypeName( OUString::createFromAscii( pEnvTypeName ) );
            
            if (bNeedsMapping)
            {
                if (! pEnv)
                    uno_getEnvironment( &pEnv, aEnvTypeName.pData, 0 );
                if (pEnv)
                {
                    OUString aCppEnvTypeName( RTL_CONSTASCII_USTRINGPARAM(CPPU_CURRENT_LANGUAGE_BINDING_NAME) );
                    uno_getEnvironment( &pCurrentEnv, aCppEnvTypeName.pData, 0 );
                    if (pCurrentEnv)
                        bNeedsMapping = (pEnv != pCurrentEnv);
                }
            }
            
            OUString aGetFactoryName( RTL_CONSTASCII_USTRINGPARAM(COMPONENT_GETFACTORY) );
            if (pSym = osl_getSymbol( lib, aGetFactoryName.pData ))
            {
                OString aImplName( OUStringToOString( rImplName, RTL_TEXTENCODING_ASCII_US ) );
                
                if (bNeedsMapping)
                {
                    if (pEnv && pCurrentEnv)
                    {
                        Mapping aCurrent2Env( pCurrentEnv, pEnv );
                        Mapping aEnv2Current( pEnv, pCurrentEnv );
                        
                        if (aCurrent2Env.is() && aEnv2Current.is())
                        {
                            void * pSMgr = aCurrent2Env.mapInterface(
                                xSF.get(), ::getCppuType( (const Reference< XMultiServiceFactory > *)0 ) );
                            void * pKey = aCurrent2Env.mapInterface(
                                xKey.get(), ::getCppuType( (const Reference< XRegistryKey > *)0 ) );
                            
                            void * pSSF = (*((component_getFactoryFunc)pSym))(
                                aImplName.getStr(), pSMgr, pKey );
                            
                            if (pKey)
                                (*pEnv->pExtEnv->releaseInterface)( pEnv->pExtEnv, pKey );
                            if (pSMgr)
                                (*pEnv->pExtEnv->releaseInterface)( pEnv->pExtEnv, pSMgr );
                            
                            if (pSSF)
                            {
                                aEnv2Current.mapInterface(
                                    reinterpret_cast< void ** >( &xRet ),
                                    pSSF, ::getCppuType( (const Reference< XSingleServiceFactory > *)0 ) );
                                (*pEnv->pExtEnv->releaseInterface)( pEnv->pExtEnv, pSSF );
                            }
                        }
                    }
                }
                else
                {
                    XSingleServiceFactory * pRet = (XSingleServiceFactory *)
                        (*((component_getFactoryFunc)pSym))(
                            aImplName.getStr(), xSF.get(), xKey.get() );
                    if (pRet)
                    {
                        xRet = pRet;
                        pRet->release();
                    }
                }
            }
            
            if (pEnv)
                (*pEnv->release)( pEnv );
            if (pCurrentEnv)
                (*pCurrentEnv->release)( pCurrentEnv );
        }
                
        if (! xRet.is())
            osl_unloadModule( lib );
    }
    
    return xRet;
}
//==================================================================================================
static Reference< ::com::sun::star::lang::XSingleServiceFactory> SAL_CALL createLoaderFactory(
    const Reference< XMultiServiceFactory > & xSF, const OUString & rBootstrapPath )
{
      return loadLibComponentFactory(
        OUString( RTL_CONSTASCII_USTRINGPARAM("cpld") ),
        OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.stoc.DLLComponentLoader") ),
        rBootstrapPath, xSF, Reference< XRegistryKey >() );
}
//==================================================================================================
static Reference< XSingleServiceFactory > SAL_CALL createSimpleRegistryFactory(
    const Reference< XMultiServiceFactory > & xSF, const OUString & rBootstrapPath )
{
    return loadLibComponentFactory(
        OUString( RTL_CONSTASCII_USTRINGPARAM("simreg") ),
        OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.stoc.SimpleRegistry") ),
        rBootstrapPath, xSF, Reference< XRegistryKey >() );
}
//==================================================================================================
static Reference< XSingleServiceFactory> SAL_CALL createDefaultRegistryFactory(
    const Reference< XMultiServiceFactory > & xSF, const OUString & rBootstrapPath )
{
    return loadLibComponentFactory(
        OUString( RTL_CONSTASCII_USTRINGPARAM("defreg") ),
        OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.stoc.DefaultRegistry") ),
        rBootstrapPath, xSF, Reference< XRegistryKey >() );
}
//==================================================================================================
static Reference< XSingleServiceFactory> SAL_CALL createNestedRegistryFactory(
    const Reference< XMultiServiceFactory > & xSF, const OUString & rBootstrapPath )
{
    return loadLibComponentFactory(
        OUString( RTL_CONSTASCII_USTRINGPARAM("defreg") ),
        OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.stoc.NestedRegistry") ),
        rBootstrapPath, xSF, Reference< XRegistryKey >() );
}
//==================================================================================================
static Reference< XSingleServiceFactory> SAL_CALL createImplRegFactory(
    const Reference< XMultiServiceFactory > & xSF, const OUString & rBootstrapPath )
{
    return loadLibComponentFactory(
        OUString( RTL_CONSTASCII_USTRINGPARAM("impreg") ),
        OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.stoc.ImplementationRegistration") ),
        rBootstrapPath, xSF, Reference< XRegistryKey >() );
}
//==================================================================================================
static Reference< XSingleServiceFactory> SAL_CALL createRegTDProviderFactory(
    const Reference< XMultiServiceFactory > & xSF, const OUString & rBootstrapPath )
{
    return loadLibComponentFactory(
        OUString( RTL_CONSTASCII_USTRINGPARAM("rdbtdp") ),
        OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.stoc.RegistryTypeDescriptionProvider") ),
        rBootstrapPath, xSF, Reference< XRegistryKey >() );
}
//==================================================================================================
static Reference< XSingleServiceFactory> SAL_CALL createTDManagerFactory(
    const Reference< XMultiServiceFactory > & xSF, const OUString & rBootstrapPath )
{
    return loadLibComponentFactory(
        OUString( RTL_CONSTASCII_USTRINGPARAM("tdmgr") ),
        OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.stoc.TypeDescriptionManager") ),
        rBootstrapPath, xSF, Reference< XRegistryKey >() );
}
//==================================================================================================
static Reference< XMultiServiceFactory > createImplServiceFactory( const OUString & rWriteRegistry, 
                                                                   const OUString & rReadRegistry,
                                                                   sal_Bool bReadOnly,
                                                                   const OUString & rBootstrapPath )
    throw( ::com::sun::star::uno::Exception )
{
    Reference< XSingleServiceFactory > xSMFac( loadLibComponentFactory(
        OUString( RTL_CONSTASCII_USTRINGPARAM("smgr") ),
        OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.stoc.ORegistryServiceManager") ),
        rBootstrapPath, Reference< XMultiServiceFactory >(), Reference< XRegistryKey >() ) );
    
    // stop here if not at least a service manager is available
    if ( !xSMFac.is() )
    {
        throw Exception( OUString(RTL_CONSTASCII_USTRINGPARAM("service manager component not available")),
                         Reference< XInterface >() );
    }
    
    Reference< XMultiServiceFactory > xSF( xSMFac->createInstance(), UNO_QUERY );
    Reference< XInitialization > xInit( xSF, UNO_QUERY );
    
    OSL_ENSHURE( xInit.is() && xSF.is(), "### failed loading servicemanager!" );
    if (xInit.is() && xSF.is())
    {
        Reference< XSingleServiceFactory > xSimRegFac( createSimpleRegistryFactory( xSF, rBootstrapPath ) );
        Reference< XSingleServiceFactory > xNesRegFac( createNestedRegistryFactory( xSF, rBootstrapPath ) );
        OSL_ENSHURE( xSimRegFac.is() && xNesRegFac.is(), "### cannot get registry factories!" );
        
        // write initial shared lib loader, simple registry, default registry, impl reg
        Reference< XSet > xSet( xSF, UNO_QUERY );
        
        // loader
        {
        Reference< XSingleServiceFactory > xFac( createLoaderFactory( xSF, rBootstrapPath ) );
        if ( !xFac.is() )
        {
            throw Exception( OUString(RTL_CONSTASCII_USTRINGPARAM("shared library loader component not available")),
                             Reference< XInterface >() );
        }
        xSet->insert( makeAny( xFac ) );
        OSL_ENSHURE( xSet->has( makeAny( xFac ) ), "### failed registering loader!" );
        }
        // simple registry
        {
        if ( !xSimRegFac.is() )
        {
            throw Exception( OUString(RTL_CONSTASCII_USTRINGPARAM("simple registry component not available")),
                             Reference< XInterface >() );
        }
        xSet->insert( makeAny( xSimRegFac ) );
        OSL_ENSHURE( xSet->has( makeAny( xSimRegFac ) ), "### failed registering simple registry!" );
        }
        // nested registry
        {
        if ( !xNesRegFac.is() )
        {
            throw Exception( OUString(RTL_CONSTASCII_USTRINGPARAM("nested registry component not available")),
                             Reference< XInterface >() );
        }
        xSet->insert( makeAny( xNesRegFac ) );
        OSL_ENSHURE( xSet->has( makeAny( xNesRegFac ) ), "### failed registering default registry!" );
        }
        // implementation registration
        {
        Reference< XSingleServiceFactory > xFac( createImplRegFactory( xSF, rBootstrapPath ) );
        if ( !xFac.is() )
        {
            throw Exception( OUString(RTL_CONSTASCII_USTRINGPARAM("implementation registration component not available")),
                             Reference< XInterface >() );
        }
        xSet->insert( makeAny( xFac ) );
        OSL_ENSHURE( xSet->has( makeAny( xFac ) ), "### failed registering impl reg!" );
        }
        
        Reference< XSimpleRegistry > xRegistry;
        
        // open a registry
        sal_Bool bRegistryShouldBeValid = sal_False;
        if (rWriteRegistry.getLength() && !rReadRegistry.getLength())
        {
            bRegistryShouldBeValid = sal_True;
            xRegistry = Reference< XSimpleRegistry >::query( xSimRegFac->createInstance() );
            if (xRegistry.is())
            {
                if (bReadOnly)
                {
                    xRegistry->open( rWriteRegistry, sal_True, sal_False );
                } else
                {
                    xRegistry->open( rWriteRegistry, sal_False, sal_True );
                }
            }
        }
        else // default registry
        if (rWriteRegistry.getLength() && rReadRegistry.getLength() )
        {
            bRegistryShouldBeValid = sal_True;
            xRegistry = Reference< XSimpleRegistry >::query( xNesRegFac->createInstance() );

            Reference< XSimpleRegistry > xWriteReg = Reference< XSimpleRegistry >::query( xSimRegFac->createInstance() );
            if ( xWriteReg.is() )
            {
                if (bReadOnly)
                {
                    try
                    {
                        xWriteReg->open( rWriteRegistry, sal_True, sal_False );
                    }
                    catch(InvalidRegistryException&)
                    {
                    }

                    if (!xWriteReg->isValid())
                    {
                        throw Exception( OUString(RTL_CONSTASCII_USTRINGPARAM("specified first registry could not be open readonly")),
                                         Reference< XInterface >() );
                    }
                } else
                {
                    xWriteReg->open( rWriteRegistry, sal_False, sal_True );
                }
            }

            Reference< XSimpleRegistry > xReadReg = Reference< XSimpleRegistry >::query( xSimRegFac->createInstance() );
            if ( xReadReg.is() )
            {
                xReadReg->open( rReadRegistry, sal_True, sal_False );
            }

            Reference< XInitialization > xInit( xRegistry, UNO_QUERY );
            Sequence< Any > aInitSeq( 2 );
            aInitSeq[0] <<= xWriteReg;
            aInitSeq[1] <<= xReadReg;
            xInit->initialize( aInitSeq );
        }			

        if (bRegistryShouldBeValid && (!xRegistry.is() || !xRegistry->isValid()))
        {
            throw Exception( OUString(RTL_CONSTASCII_USTRINGPARAM("specified registry could not be initialized")),
                             Reference< XInterface >() );
        }
        
//		OSL_ENSHURE( xRegistry.is(), "### got no registry!" );
        Reference< XInterface > xInst;
        if (xRegistry.is())
        {
            // init service factory with registry
            Sequence< Any > aSeq( 1 );
            // registry to work on
            aSeq[0] = makeAny( xRegistry );
            xInit->initialize( aSeq );

            xInst = xSF->createInstance(  rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.reflection.TypeDescriptionManager")) );
        }			

        // initial td manager held by service manager
        if (! xInst.is())
        {
            // td registry provider registration
            {
                Reference< XSingleServiceFactory > xFac( createRegTDProviderFactory( xSF, rBootstrapPath ) );
                if ( !xFac.is() )
                {
                    throw Exception( OUString(RTL_CONSTASCII_USTRINGPARAM("type description provider component not available")),
                                     Reference< XInterface >() );
                }
                xSet->insert( makeAny( xFac ) );
                OSL_ENSHURE( xSet->has( makeAny( xFac ) ), "### failed registering registry td provider!" );
            }
            // stoc td manager registration
            {
                Reference< XSingleServiceFactory > xFac( createTDManagerFactory( xSF, rBootstrapPath ) );
                if ( !xFac.is() )
                {
                    throw Exception( OUString(RTL_CONSTASCII_USTRINGPARAM("type description manager component not available")),
                                     Reference< XInterface >() );
                }
                xSet->insert( makeAny( xFac ) );
                OSL_ENSHURE( xSet->has( makeAny( xFac ) ), "### failed registering td manager!" );
            }
            xInst = xSF->createInstance( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.reflection.TypeDescriptionManager")) );
        }
        if (xInst.is())
        {
            Reference< XInitialization > xTDMInit( xInst, UNO_QUERY );
            if (xTDMInit.is())
                xTDMInit->initialize( Sequence< Any >() );
        } else
        {
            throw Exception( OUString(RTL_CONSTASCII_USTRINGPARAM("service \"com.sun.star.reflection.TypeDescriptionManager\" could not be initialized")),
                             Reference< XInterface >() );
        }
        return xSF;
    }
    return Reference< XMultiServiceFactory >();
}

//==================================================================================================
Reference< XMultiServiceFactory > SAL_CALL createRegistryServiceFactory( const OUString & rWriteRegistry, 
                                                                         const OUString & rReadRegistry,
                                                                         sal_Bool bReadOnly,
                                                                         const OUString & rBootstrapPath )
    throw( ::com::sun::star::uno::Exception )
{
    return createImplServiceFactory( rWriteRegistry, rReadRegistry, bReadOnly, rBootstrapPath );
}

}

