/*************************************************************************
 *
 *  $RCSfile: bootstrap.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: dbo $ $Date: 2002-06-14 13:20:19 $
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

#include <string.h>
#include <vector>

#include <rtl/process.h>
#include <rtl/bootstrap.hxx>
#include <rtl/string.hxx>
#include <rtl/ustrbuf.hxx>
#ifdef _DEBUG
#include <rtl/strbuf.hxx>
#endif

#include <osl/diagnose.h>
#include <osl/file.hxx>
#include <osl/module.hxx>
#include <osl/security.hxx>
#include <osl/thread.h>

#include <uno/current_context.h>

#include <cppuhelper/shlib.hxx>
#include <cppuhelper/bootstrap.hxx>
#include <cppuhelper/component_context.hxx>
#include <cppuhelper/access_control.hxx>

#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/uno/XCurrentContext.hpp>

#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/lang/XSingleComponentFactory.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>

#include <com/sun/star/registry/XSimpleRegistry.hpp>
#include <com/sun/star/container/XSet.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>

#define OUSTR(x) ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(x) )


using namespace ::rtl;
using namespace ::osl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

namespace cppu
{

static OUString str_envType = OUSTR(CPPU_CURRENT_LANGUAGE_BINDING_NAME);

//==================================================================================================
void * SAL_CALL parentThreadCallback(void) SAL_THROW_EXTERN_C()
{
    OSL_TRACE( "+> thread creation..." );
    XCurrentContext * xContext = 0;
    ::uno_getCurrentContext( (void **)&xContext, str_envType.pData, 0 );
    return xContext; // return acquired context
}
//==================================================================================================
void SAL_CALL childThreadCallback( void * pParentData ) SAL_THROW_EXTERN_C()
{
    OSL_TRACE( "++> child thread running." );
    XCurrentContext * xContext = (XCurrentContext *)pParentData;
    if (xContext)
    {
        ::uno_setCurrentContext( xContext, str_envType.pData, 0 );
        xContext->release();
    }
}

//==================================================================================================
void addFactories(
    char const * const * ppNames /* lib, implname, ..., 0 */,
    OUString const & bootstrapPath,
    Reference< lang::XMultiComponentFactory > const & xMgr,
    Reference< registry::XRegistryKey > const & xKey )
    SAL_THROW( (Exception) )
{
    Reference< container::XSet > xSet( xMgr, UNO_QUERY );
    OSL_ASSERT( xSet.is() );
    Reference< lang::XMultiServiceFactory > xSF( xMgr, UNO_QUERY );
    
    while (*ppNames)
    {
        OUString lib( OUString::createFromAscii( *ppNames++ ) );
        OUString implName( OUString::createFromAscii( *ppNames++ ) );
        
        Any aFac( makeAny( loadSharedLibComponentFactory( lib, bootstrapPath, implName, xSF, xKey ) ) );
        xSet->insert( aFac );
#ifdef DEBUG
        if (xSet->has( aFac ))
        {
            Reference< lang::XServiceInfo > xInfo;
            if (aFac >>= xInfo)
            {
                ::fprintf( stderr, "> implementation %s supports: ", ppNames[ -1 ] );
                Sequence< OUString > supported( xInfo->getSupportedServiceNames() );
                for ( sal_Int32 nPos = supported.getLength(); nPos--; )
                {
                    OString str( OUStringToOString(
                        supported[ nPos ], RTL_TEXTENCODING_ASCII_US ) );
                    ::fprintf( stderr, nPos ? "%s, " : "%s\n", str.getStr() );
                }
            }
            else
            {
                ::fprintf(
                    stderr,
                    "> implementation %s provides NO lang::XServiceInfo!!!\n", ppNames[ -1 ] );
            }
        }
#endif
#ifdef _DEBUG
        if (! xSet->has( aFac ))
        {
            OStringBuffer buf( 64 );
            buf.append( "### failed inserting shared lib \"" );
            buf.append( ppNames[ -2 ] );
            buf.append( "\"!!!" );
            OString str( buf.makeStringAndClear() );
            OSL_ENSURE( 0, str.getStr() );
        }
#endif
    }
}

// private forward decl
//--------------------------------------------------------------------------------------------------
Reference< lang::XMultiComponentFactory > bootstrapInitialSF(
    OUString const & rBootstrapPath )
    SAL_THROW( (Exception) );
//--------------------------------------------------------------------------------------------------
Reference< XComponentContext > bootstrapInitialContext(
    Reference< lang::XMultiComponentFactory > const & xSF,
    Reference< registry::XSimpleRegistry > const & types_xRegistry,
    Reference< registry::XSimpleRegistry > const & services_xRegistry,
    OUString const & rBootstrapPath, Bootstrap const & bootstrap )
    SAL_THROW( (Exception) );

//--------------------------------------------------------------------------------------------------
Reference< XComponentContext > SAL_CALL createInitialCfgComponentContext(
    ContextEntry_Init const * pEntries, sal_Int32 nEntries,
    Reference< XComponentContext > const & xDelegate )
    SAL_THROW( () );
//--------------------------------------------------------------------------------------------------
Reference< registry::XSimpleRegistry > SAL_CALL createRegistryWrapper(
    const Reference< XComponentContext >& xContext );

//--------------------------------------------------------------------------------------------------
template< class T >
static inline beans::PropertyValue createPropertyValue(
    OUString const & name, T const & value )
    SAL_THROW( () )
{
    return beans::PropertyValue( name, -1, makeAny( value ), beans::PropertyState_DIRECT_VALUE );
}

//--------------------------------------------------------------------------------------------------
static OUString findBoostrapArgument(
    const Bootstrap & bootstrap,
    const OUString & arg_name,
    sal_Bool * pFallenBack )
    SAL_THROW(())
{
    OUString result;

    OUString prefixed_arg_name = OUString(RTL_CONSTASCII_USTRINGPARAM("UNO_"));
    prefixed_arg_name += arg_name.toAsciiUpperCase();

    // environment not set -> try relative to executable
    if(!bootstrap.getFrom(prefixed_arg_name, result))
    {
        if(pFallenBack)
            *pFallenBack = sal_True;

        OUString fileName;
        bootstrap.getIniName(fileName);
        
        // cut the rc extension
        result = fileName.copy(0, fileName.getLength() - strlen(SAL_CONFIGFILE("")));
        result += OUString(RTL_CONSTASCII_USTRINGPARAM("_"));
        result += arg_name.toAsciiLowerCase();
        result += OUString(RTL_CONSTASCII_USTRINGPARAM(".rdb"));
        
#ifdef DEBUG
        OString result_dbg = OUStringToOString(result, RTL_TEXTENCODING_ASCII_US);
        OString arg_name_dbg = OUStringToOString(arg_name, RTL_TEXTENCODING_ASCII_US);
        OSL_TRACE("cppuhelper::findBoostrapArgument - setting %s relative to executable: %s\n",
                  arg_name_dbg.getStr(),
                  result_dbg.getStr());
#endif
    }
    else
    {
        if(pFallenBack)
            *pFallenBack = sal_False;

#ifdef DEBUG
        OString prefixed_arg_name_dbg = OUStringToOString(
            prefixed_arg_name, RTL_TEXTENCODING_ASCII_US );
        OString result_dbg = OUStringToOString(
            result, RTL_TEXTENCODING_ASCII_US );
        OSL_TRACE(
            "cppuhelper::findBoostrapArgument - found %s in env: %s",
            prefixed_arg_name_dbg.getStr(),
            result_dbg.getStr() );
#endif
    }
    
    return result;
}

//--------------------------------------------------------------------------------------------------
static Reference< registry::XSimpleRegistry > nestRegistries(
    const OUString baseDir,
    const Reference< lang::XSingleServiceFactory > & xSimRegFac,
    const Reference< lang::XSingleServiceFactory > & xNesRegFac,
    OUString csl_rdbs,
    const OUString & write_rdb,
    sal_Bool forceWrite_rdb,
    sal_Bool bFallenBack )
    SAL_THROW((Exception))
{
    sal_Int32 index;
    Reference< registry::XSimpleRegistry > lastRegistry;

    if(write_rdb.getLength()) // is there a write registry given?
    {
        lastRegistry.set(xSimRegFac->createInstance(), UNO_QUERY);

        try
        {
            OSL_TRACE("opening xxxxx");
              lastRegistry->open(write_rdb, sal_False, forceWrite_rdb);
            OSL_TRACE("opening yyyy");

        }
        catch (registry::InvalidRegistryException & invalidRegistryException)
        {
#ifdef DEBUG
            OString rdb_name_tmp = OUStringToOString(
                write_rdb, RTL_TEXTENCODING_ASCII_US);
            OString message_dbg = OUStringToOString(
                invalidRegistryException.Message, RTL_TEXTENCODING_ASCII_US);
            OSL_TRACE(
                "warning: couldn't open %s cause of %s",
                rdb_name_tmp.getStr(), message_dbg.getStr() );
#endif
        }

        if(!lastRegistry->isValid())
            lastRegistry.clear();
    }

    do
    {
        index = csl_rdbs.indexOf((sal_Unicode)' ');
        OUString rdb_name = (index == -1) ? csl_rdbs : csl_rdbs.copy(0, index);
        csl_rdbs = (index == -1) ? OUString() : csl_rdbs.copy(index + 1);

        if (! rdb_name.getLength())
            continue;
        
        bool optional = ('?' == rdb_name[ 0 ]);
        if (optional)
            rdb_name = rdb_name.copy( 1 );
        
        try
        {
            Reference<registry::XSimpleRegistry> simpleRegistry(
                xSimRegFac->createInstance(), UNO_QUERY );
            
            osl::FileBase::getAbsoluteFileURL(baseDir, rdb_name, rdb_name);
            simpleRegistry->open(rdb_name, sal_True, sal_False);

            if(lastRegistry.is())
            {
                Reference< registry::XSimpleRegistry > nestedRegistry(
                    xNesRegFac->createInstance(), UNO_QUERY );
                Reference< lang::XInitialization > nestedRegistry_xInit(
                    nestedRegistry, UNO_QUERY );
                
                Sequence<Any> aArgs(2);
                aArgs[0] <<= lastRegistry;
                aArgs[1] <<= simpleRegistry;

                nestedRegistry_xInit->initialize(aArgs);

                lastRegistry = nestedRegistry;
            }
            else
                lastRegistry = simpleRegistry;
        }
        catch(registry::InvalidRegistryException & invalidRegistryException)
        {
            if (! optional)
            {
                // if a registry was explicitly given, the exception shall fly
                if( ! bFallenBack )
                    throw;
            }

#ifdef DEBUG
            OString rdb_name_tmp = OUStringToOString(
                rdb_name, RTL_TEXTENCODING_ASCII_US );
            OString message_dbg = OUStringToOString(
                invalidRegistryException.Message, RTL_TEXTENCODING_ASCII_US );
            OSL_TRACE(
                "warning: couldn't open %s cause of %s",
                rdb_name_tmp.getStr(), message_dbg.getStr() );
#endif
        }
    }
    while(index != -1 && csl_rdbs.getLength()); // are there more rdbs in list?
    
    return lastRegistry;
}

//--------------------------------------------------------------------------------------------------
static Reference< XComponentContext > SAL_CALL __defaultBootstrap_InitialComponentContext(
    Bootstrap const & bootstrap )
    SAL_THROW( (Exception) )
{
//      osl_registerThreadCallbacks( parentThreadCallback, childThreadCallback );
    
    OUString bootstrapPath;
    OUString iniDir;

    osl_getProcessWorkingDir(&iniDir.pData);
    
    Reference<lang::XMultiComponentFactory> smgr_XMultiComponentFactory(
        bootstrapInitialSF(bootstrapPath) );
    Reference<lang::XMultiServiceFactory> smgr_XMultiServiceFactory(
        smgr_XMultiComponentFactory, UNO_QUERY );
    
    Reference<registry::XRegistryKey> xEmptyKey;
    Reference<lang::XSingleServiceFactory> xSimRegFac(
        loadSharedLibComponentFactory(
            OUString(RTL_CONSTASCII_USTRINGPARAM("simreg")),
            bootstrapPath,
            OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.stoc.SimpleRegistry")),
            smgr_XMultiServiceFactory,
            xEmptyKey),
        UNO_QUERY);
    
    Reference<lang::XSingleServiceFactory> xNesRegFac(
        loadSharedLibComponentFactory(
            OUString( RTL_CONSTASCII_USTRINGPARAM("defreg")),
            bootstrapPath,
            OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.stoc.NestedRegistry")),
            smgr_XMultiServiceFactory,
            xEmptyKey),
        UNO_QUERY);
    
    sal_Bool bFallenback_types;
    OUString cls_uno_types = findBoostrapArgument(
        bootstrap, OUString(RTL_CONSTASCII_USTRINGPARAM("TYPES")), &bFallenback_types );
    
    Reference<registry::XSimpleRegistry> types_xRegistry = nestRegistries(
        iniDir, xSimRegFac, xNesRegFac, cls_uno_types, OUString(), sal_False, bFallenback_types );

/*
    // xxx todo: when moving down cfgmgr+interfaces to udk, code from cfg_registry_wrapper
    // is used.  Now not supported...
    OUString cfg_url;
    if (bootstrap.getFrom( OUSTR("UNO_CFG_URL"), cfg_url ))
    {
        // ==== bootstrap from configuration ====
        
        // servertype;sourcepath[;updatepath]
        sal_Int32 semi = cfg_url.indexOf( ';', 0 );
        OSL_ENSURE( semi >= 0, "# invalid UNO_CFG_URL!" );
        if (semi < 0)
        {
            throw Exception( OUSTR( "invalid UNO_CFG_URL" ), Reference< XInterface >() );
        }
        
        // xxx todo lib names work with src libs: sax, cfgmgr2!!!
        // add additional factories for config provider
        static char const * ar[] = {
            "sax", "com.sun.star.comp.extensions.xml.sax.ParserExpat",
            "sax", "com.sun.star.extensions.xml.sax.Writer",
//              "stm", "com.sun.star.comp.io.stm.Pipe",
//              "stm", "com.sun.star.comp.io.stm.DataInputStream",
//              "stm", "com.sun.star.comp.io.stm.DataOutputStream",
//              "stm", "com.sun.star.comp.io.stm.ObjectInputStream",
//              "stm", "com.sun.star.comp.io.stm.MarkableInputStream",
//              "stm", "com.sun.star.comp.stoc.Pump",
            "stm", "com.sun.star.comp.io.stm.MarkableOutputStream",
            "stm", "com.sun.star.comp.io.stm.ObjectOutputStream",
            "tcv", "com.sun.star.comp.stoc.TypeConverter",
            "cfgmgr2", "com.sun.star.comp.configuration.ConfigurationProvider",
            "cfgmgr2", "com.sun.star.comp.configuration.AdministrationProvider",
            0
        };
        addFactories( ar, bootstrapPath, smgr_XMultiComponentFactory, xEmptyKey );
        
        ContextEntry_Init entry;
        ::std::vector< ContextEntry_Init > context_values;
        context_values.reserve( 2 );
        
        // singleton entry
        entry.bLateInitService = true;
        entry.name = OUSTR("/singletons/com.sun.star.bootstrap.theConfigurationProvider");
        sal_Int32 semi2 = cfg_url.indexOf( ';', semi +1 );
        if (semi2 > semi) // use config provider
        {
            // xxx todo: service name would suffice here, if registered factory (bootstrap.cxx)
            // would export lang::XServiceInfo...
            entry.value <<= loadSharedLibComponentFactory(
                OUSTR("cfgmgr2"), bootstrapPath,
                OUSTR("com.sun.star.comp.configuration.ConfigurationProvider"),
                smgr_XMultiServiceFactory, Reference< registry::XRegistryKey >() );
            context_values.push_back( entry );
            
            Sequence< Any > cfg_args( 3 );
            cfg_args[ 0 ] <<= createPropertyValue(
                OUSTR("servertype"), cfg_url.copy( 0, semi ) );
            cfg_args[ 1 ] <<= createPropertyValue(
                OUSTR("sourcepath"), cfg_url.copy( semi +1, semi2 -semi -1 ) );
            cfg_args[ 2 ] <<= createPropertyValue(
                OUSTR("updatepath"), cfg_url.copy( semi2 +1 ) );
            entry.value <<= cfg_args;
        }
        else // use admin provider
        {
            // xxx todo: service name would suffice here, if registered factory (bootstrap.cxx)
            // would export lang::XServiceInfo...
            entry.value <<= loadSharedLibComponentFactory(
                OUSTR("cfgmgr2"), bootstrapPath,
                OUSTR("com.sun.star.comp.configuration.AdministrationProvider"),
                smgr_XMultiServiceFactory, Reference< registry::XRegistryKey >() );
            context_values.push_back( entry );
            
            Sequence< Any > cfg_args( 2 );
            cfg_args[ 0 ] <<= createPropertyValue(
                OUSTR("servertype"), cfg_url.copy( 0, semi ) );
            cfg_args[ 1 ] <<= createPropertyValue(
                OUSTR("sourcepath"), cfg_url.copy( semi +1 ) );
            entry.value <<= cfg_args;
        }
        entry.bLateInitService = false;
        entry.name = OUSTR("/singletons/com.sun.star.bootstrap.theConfigurationProvider/initial-arguments");
        context_values.push_back( entry );
        
        // layer into two contexts
        Reference< XComponentContext > xContext( bootstrapInitialContext(
            smgr_XMultiComponentFactory, types_xRegistry,
            Reference< registry::XSimpleRegistry >(),
            bootstrapPath, bootstrap ) );
        xContext = createInitialCfgComponentContext(
            &context_values[ 0 ], context_values.size(), xContext );
        
        // initialize sf
        Reference< lang::XInitialization > xInit( smgr_XMultiComponentFactory, UNO_QUERY );
        OSL_ASSERT( xInit.is() );
        Sequence< Any > aSFInit( 1 );
        aSFInit[ 0 ] <<= createRegistryWrapper( xContext );
        // for now the registry service manager works upon the wrapped config,
        // we will proceed implementing a config service manager when everything works fine...
        xInit->initialize( aSFInit );
        
        return xContext;
    }
    else
*/
    {
        // ==== bootstrap from services registry ====

        sal_Bool bFallenback_services;
        OUString cls_uno_services = findBoostrapArgument(
            bootstrap, OUString(RTL_CONSTASCII_USTRINGPARAM("SERVICES")), &bFallenback_services );
        
        sal_Bool fallenBackWriteRegistry;
        OUString write_rdb = findBoostrapArgument(
            bootstrap, OUString(RTL_CONSTASCII_USTRINGPARAM("WRITERDB")), &fallenBackWriteRegistry );
        
        Reference<registry::XSimpleRegistry> services_xRegistry = nestRegistries(
            iniDir, xSimRegFac, xNesRegFac, cls_uno_services, write_rdb,
            !fallenBackWriteRegistry, bFallenback_services );
        
        Reference< XComponentContext > xContext( bootstrapInitialContext(
            smgr_XMultiComponentFactory, types_xRegistry, services_xRegistry,
            bootstrapPath, bootstrap ) );
        
        // initialize sf
        Reference< lang::XInitialization > xInit( smgr_XMultiComponentFactory, UNO_QUERY );
        OSL_ASSERT( xInit.is() );
        Sequence< Any > aSFInit( 1 );
        aSFInit[ 0 ] <<= services_xRegistry;
        xInit->initialize( aSFInit );
        
        return xContext;
    }
}

//##################################################################################################
//##################################################################################################
//##################################################################################################

static void MyDummySymbolWithinLibrary(){}
//--------------------------------------------------------------------------------------------------
Bootstrap const & __get_unorc() SAL_THROW( () )
{
    static rtlBootstrapHandle s_bstrap = 0;
    if (! s_bstrap)
    {
        OUString libraryFileUrl;
        Module::getUrlFromAddress((void*)MyDummySymbolWithinLibrary, libraryFileUrl);
        
        OUString iniName = libraryFileUrl.copy(0, libraryFileUrl.lastIndexOf((sal_Unicode)'/') + 1); // cut the library extension
        iniName += OUString(RTL_CONSTASCII_USTRINGPARAM(SAL_CONFIGFILE("uno"))); // add the rc file extension
        rtlBootstrapHandle bstrap = rtl_bootstrap_args_open( iniName.pData );
        ClearableMutexGuard guard( Mutex::getGlobalMutex() );
        if (s_bstrap)
        {
            guard.clear();
            rtl_bootstrap_args_close( bstrap );
        }
        else
        {
            s_bstrap = bstrap;
        }
    }
    return *(Bootstrap const *)&s_bstrap;
}

//==================================================================================================
Reference< XComponentContext > SAL_CALL defaultBootstrap_InitialComponentContext(
    OUString const & iniFile )
    SAL_THROW( (Exception) )
{
    Bootstrap bootstrap( iniFile );
    return __defaultBootstrap_InitialComponentContext( bootstrap );
}
//==================================================================================================
Reference< XComponentContext > SAL_CALL defaultBootstrap_InitialComponentContext()
    SAL_THROW( (Exception) )
{
    return __defaultBootstrap_InitialComponentContext( __get_unorc() );
}

} // namespace cppu

