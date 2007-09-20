/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: shlib.cxx,v $
 *
 *  $Revision: 1.29 $
 *
 *  last change: $Author: vg $ $Date: 2007-09-20 14:26:04 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_cppuhelper.hxx"

#include "osl/diagnose.h"
#include "osl/file.hxx"
#include "osl/mutex.hxx"
#include "osl/module.hxx"
#include "rtl/unload.h"
#include "rtl/ustrbuf.hxx"
#include "uno/environment.h"
#include "uno/mapping.hxx"
#include "cppuhelper/factory.hxx"
#include "cppuhelper/shlib.hxx"

#include "com/sun/star/beans/XPropertySet.hpp"

#if OSL_DEBUG_LEVEL > 1
#include <stdio.h>
#endif
#include <vector>

#define OUSTR(x) ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(x) )


using namespace ::rtl;
using namespace ::osl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

namespace cppu
{

#if OSL_DEBUG_LEVEL > 1
//------------------------------------------------------------------------------
static inline void out( const char * p ) SAL_THROW( () )
{
    printf( p );
}
static inline void out( const OUString & r ) throw ()
{
    OString s( OUStringToOString( r, RTL_TEXTENCODING_ASCII_US ) );
    out( s.getStr() );
}
#endif

//------------------------------------------------------------------------------
static const ::std::vector< OUString > * getAccessDPath() SAL_THROW( () )
{
    static ::std::vector< OUString > * s_p = 0;
    static bool s_bInit = false;

    if (! s_bInit)
    {
        ::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
        if (! s_bInit)
        {
            const char * pEnv = ::getenv( "CPLD_ACCESSPATH" );
            if (pEnv)
            {
                static ::std::vector< OUString > s_v;

                OString aEnv( pEnv );
                sal_Int32 nIndex = 0;
                do
                {
                    OUString aStr( OStringToOUString(
                        aEnv.getToken( 0, ';', nIndex ),
                        RTL_TEXTENCODING_ASCII_US ) );
                    OUString aFileUrl;
                    if (FileBase::getFileURLFromSystemPath(aStr, aFileUrl)
                        != FileBase::E_None)
                    {
                        OSL_ASSERT(false);
                    }
                    s_v.push_back( aFileUrl );
                } while( nIndex != -1 );
#if OSL_DEBUG_LEVEL > 1
                out( "> cpld: acknowledged following access path(s): \"" );
                ::std::vector< OUString >::const_iterator iPos( s_v.begin() );
                while (iPos != s_v.end())
                {
                    out( *iPos );
                    ++iPos;
                    if (iPos != s_v.end())
                        out( ";" );
                }
                out( "\"\n" );
#endif
                s_p = & s_v;
            }
            else
            {
                // no access path env set
#if OSL_DEBUG_LEVEL > 1
                out( "=> no CPLD_ACCESSPATH set.\n" );
#endif
            }
            s_bInit = true;
        }
    }

    return s_p;
}

//------------------------------------------------------------------------------
static bool checkAccessPath( OUString * pComp ) throw ()
{
    const ::std::vector< OUString > * pPath = getAccessDPath();

    if (pPath)
    {
        sal_Bool bAbsolute = (pComp->compareToAscii( "file://" , 7 ) == 0);
        for ( ::std::vector< OUString >::const_iterator iPos( pPath->begin() );
              iPos != pPath->end(); ++iPos )
        {
            OUString aBaseDir( *iPos );
            OUString aAbs;

            if ( bAbsolute )
            {
                aAbs = *pComp;
#if OSL_DEBUG_LEVEL > 1
                out( "> taking path: \"" );
                out( aAbs );
#endif
            }
            else
            {
                if (osl_File_E_None !=
                    ::osl_getAbsoluteFileURL(
                        aBaseDir.pData, pComp->pData, &aAbs.pData ))
                {
                    continue;
                }
#if OSL_DEBUG_LEVEL > 1
                out( "> found path: \"" );
                out( aBaseDir );
                out( "\" + \"" );
                out( *pComp );
                out( "\" => \"" );
                out( aAbs );
#endif
            }

            if (0 == aAbs.indexOf( aBaseDir ) && // still part of it?
                aBaseDir.getLength() < aAbs.getLength() &&
                (aBaseDir[ aBaseDir.getLength() -1 ] == (sal_Unicode)'/' ||
                 // dir boundary
                 aAbs[ aBaseDir.getLength() ] == (sal_Unicode)'/'))
            {
#if OSL_DEBUG_LEVEL > 1
                out( ": ok.\n" );
#endif
                // load from absolute path
                *pComp = aAbs;
                return true;
            }
#if OSL_DEBUG_LEVEL > 1
            else
            {
                out( "\" ...does not match given path \"" );
                out( aBaseDir );
                out( "\".\n" );
            }
#endif
        }
        return false;
    }
    else
    {
        // no access path env set
        return true;
    }
}

//------------------------------------------------------------------------------
static inline sal_Int32 endsWith(
    const OUString & rText, const OUString & rEnd ) SAL_THROW( () )
{
    if (rText.getLength() >= rEnd.getLength() &&
        rEnd.equalsIgnoreAsciiCase(
            rText.copy( rText.getLength() - rEnd.getLength() ) ))
    {
        return rText.getLength() - rEnd.getLength();
    }
    return -1;
}

//------------------------------------------------------------------------------
static OUString makeComponentPath(
    const OUString & rLibName, const OUString & rPath )
{
#if OSL_DEBUG_LEVEL > 0
    // No system path allowed here !
    {
        OUString aComp;
        OSL_ASSERT( FileBase::E_None ==
                    FileBase::getSystemPathFromFileURL( rLibName, aComp ) );
        OSL_ASSERT(
            ! rPath.getLength() ||
            FileBase::E_None ==
              FileBase::getSystemPathFromFileURL( rPath, aComp ) );
    }
#endif

    OUStringBuffer buf( rPath.getLength() + rLibName.getLength() + 12 );

    if (0 != rPath.getLength())
    {
        buf.append( rPath );
        if (rPath[ rPath.getLength() -1 ] != '/')
            buf.append( (sal_Unicode) '/' );
    }
    sal_Int32 nEnd = endsWith( rLibName, OUSTR(SAL_DLLEXTENSION) );
    if (nEnd < 0) // !endsWith
    {
#ifndef OS2
//this is always triggered with .uno components
#if (OSL_DEBUG_LEVEL >= 2)
        OSL_ENSURE(
            !"### library name has no proper extension!",
            OUStringToOString( rLibName, RTL_TEXTENCODING_ASCII_US ).getStr() );
#endif
#endif // OS2

#if defined SAL_DLLPREFIX
        nEnd = endsWith( rLibName, OUSTR(".uno") );
        if (nEnd < 0) // !endsWith
            buf.appendAscii( RTL_CONSTASCII_STRINGPARAM(SAL_DLLPREFIX) );
#endif
        buf.append( rLibName );
        buf.appendAscii( RTL_CONSTASCII_STRINGPARAM(SAL_DLLEXTENSION) );
    }
    else // name is completely pre/postfixed
    {
        buf.append( rLibName );
    }

    OUString out( buf.makeStringAndClear() );
#if OSL_DEBUG_LEVEL > 1
    OString str( OUStringToOString( out, RTL_TEXTENCODING_ASCII_US ) );
    OSL_TRACE( "component path=%s\n", str.getStr() );
#endif

    return out;
}

//==============================================================================
static OUString getLibEnv(OUString         const & aModulePath,
                          oslModule                lib, 
                          uno::Environment       * pEnv, 
                          OUString               * pSourceEnv_name,
                          uno::Environment const & cTargetEnv,
                          OUString         const & cImplName = OUString())
{
    OUString aExcMsg;

    sal_Char const * pEnvTypeName = NULL;

    OUString aGetEnvNameExt = OUSTR(COMPONENT_GETENVEXT);
    component_getImplementationEnvironmentExtFunc pGetImplEnvExt = 
        (component_getImplementationEnvironmentExtFunc)osl_getFunctionSymbol(lib, aGetEnvNameExt.pData);

    if (pGetImplEnvExt)
    {
        OString implName(OUStringToOString(cImplName, RTL_TEXTENCODING_ASCII_US));
        pGetImplEnvExt(&pEnvTypeName, (uno_Environment **)pEnv, implName.getStr(), cTargetEnv.get());
    }
    else
    {
        OUString aGetEnvName = OUSTR(COMPONENT_GETENV);
        component_getImplementationEnvironmentFunc pGetImplEnv = 
            (component_getImplementationEnvironmentFunc)osl_getFunctionSymbol( 
                lib, aGetEnvName.pData );
        if (pGetImplEnv)
            pGetImplEnv(&pEnvTypeName, (uno_Environment **)pEnv);

        else
        {
            aExcMsg = aModulePath;
            aExcMsg += OUSTR(": cannot get symbol: ");
            aExcMsg += aGetEnvName;
            aExcMsg += OUSTR("- nor: ");
        }
    }
    
    if (!pEnv->is() && pEnvTypeName)
        *pSourceEnv_name = OUString::createFromAscii(pEnvTypeName);

    return aExcMsg;
}

extern "C" {static void s_getFactory(va_list * pParam) 
{
    component_getFactoryFunc         pSym      = va_arg(*pParam, component_getFactoryFunc);
    OString                  const * pImplName = va_arg(*pParam, OString const *);
    void                           * pSMgr     = va_arg(*pParam, void *);
    void                           * pKey      = va_arg(*pParam, void *);
    void                          ** ppSSF     = va_arg(*pParam, void **);

    *ppSSF = pSym(pImplName->getStr(), pSMgr, pKey);
}}

Reference< XInterface > SAL_CALL loadSharedLibComponentFactory(
    OUString const & rLibName, OUString const & rPath,
    OUString const & rImplName,
    Reference< lang::XMultiServiceFactory > const & xMgr,
    Reference< registry::XRegistryKey > const & xKey )
    SAL_THROW( (loader::CannotActivateFactoryException) )
{
    OUString aModulePath( makeComponentPath( rLibName, rPath ) );
    if (! checkAccessPath( &aModulePath ))
    {
        throw loader::CannotActivateFactoryException(
            OUSTR("permission denied to load component library: ") +
            aModulePath,
            Reference< XInterface >() );
    }
    
    oslModule lib = osl_loadModule(
        aModulePath.pData, SAL_LOADMODULE_LAZY | SAL_LOADMODULE_GLOBAL );
    if (! lib)
    {
        throw loader::CannotActivateFactoryException(
            OUSTR("loading component library failed: ") + aModulePath,
            Reference< XInterface >() );
    }

    Reference< XInterface > xRet;

    uno::Environment currentEnv(Environment::getCurrent());
    uno::Environment env;

    OUString aEnvTypeName;

    OUString aExcMsg = getLibEnv(aModulePath, lib, &env, &aEnvTypeName, currentEnv, rImplName);
    if (!aExcMsg.getLength())
    {
        OUString aGetFactoryName = OUSTR(COMPONENT_GETFACTORY);
        oslGenericFunction pSym = osl_getFunctionSymbol( lib, aGetFactoryName.pData );
        if (pSym != 0)
        {
            OString aImplName(
                OUStringToOString( rImplName, RTL_TEXTENCODING_ASCII_US ) );

            if (!env.is())
                env = uno::Environment(aEnvTypeName);
            
            if (env.is() && currentEnv.is())
            {
#if OSL_DEBUG_LEVEL > 1
                {
                    rtl::OString libName(rtl::OUStringToOString(rLibName, RTL_TEXTENCODING_ASCII_US));
                    rtl::OString implName(rtl::OUStringToOString(rImplName, RTL_TEXTENCODING_ASCII_US));
                    rtl::OString envDcp(rtl::OUStringToOString(env.getTypeName(), RTL_TEXTENCODING_ASCII_US));
                    
                    fprintf(stderr, "loadSharedLibComponentFactory envDcp: %-12.12s  implName: %30.30s  libName: %-15.15s\n", envDcp.getStr(), implName.getStr() + (implName.getLength() > 30 ? implName.getLength() - 30 : 0), libName.getStr());
                }
#endif

                Mapping aCurrent2Env( currentEnv, env );
                Mapping aEnv2Current( env, currentEnv );
                
                if (aCurrent2Env.is() && aEnv2Current.is())
                {
                    void * pSMgr = aCurrent2Env.mapInterface(
                        xMgr.get(), ::getCppuType( &xMgr ) );
                    void * pKey = aCurrent2Env.mapInterface(
                        xKey.get(), ::getCppuType( &xKey ) );
                    
                    void * pSSF = NULL;
                    
                    env.invoke(s_getFactory, pSym, &aImplName, pSMgr, pKey, &pSSF);

                    if (pKey)
                    {
                        (env.get()->pExtEnv->releaseInterface)(
                            env.get()->pExtEnv, pKey );
                    }
                    if (pSMgr)
                    {
                        (*env.get()->pExtEnv->releaseInterface)(
                            env.get()->pExtEnv, pSMgr );
                    }
                    
                    if (pSSF)
                    {
                        aEnv2Current.mapInterface(
                            reinterpret_cast< void ** >( &xRet ),
                            pSSF, ::getCppuType( &xRet ) );
                        (env.get()->pExtEnv->releaseInterface)(
                            env.get()->pExtEnv, pSSF );
                    }
                    else
                    {
                        aExcMsg = aModulePath;
                        aExcMsg += OUSTR(": cannot get factory of "
                                         "demanded implementation: ");
                        aExcMsg += OStringToOUString(
                                aImplName, RTL_TEXTENCODING_ASCII_US );
                    }
                }
                else
                {
                    aExcMsg =
                        OUSTR("cannot get uno mappings: C++ <=> UNO!");
                }
            }
            else
            {
                aExcMsg = OUSTR("cannot get uno environments!");
            }
        }
        else
        {
            aExcMsg = aModulePath;
            aExcMsg += OUSTR(": cannot get symbol: ");
            aExcMsg += aGetFactoryName;
        }
    }

    if (! xRet.is())
    {
        osl_unloadModule( lib );
#if OSL_DEBUG_LEVEL > 1
        out( "### cannot activate factory: " );
        out( aExcMsg );
        out( "\n" );
#endif
        throw loader::CannotActivateFactoryException(
            aExcMsg,
            Reference< XInterface >() );
    }

    rtl_registerModuleForUnloading( lib);
    return xRet;
}

//==============================================================================
extern "C" { static void s_writeInfo(va_list * pParam) 
{
    component_writeInfoFunc         pSym      = va_arg(*pParam, component_writeInfoFunc);
    void                          * pSMgr     = va_arg(*pParam, void *);
    void                          * pKey      = va_arg(*pParam, void *);
    sal_Bool                      * pbRet     = va_arg(*pParam, sal_Bool *);

    *pbRet = pSym(pSMgr, pKey);

}}

void SAL_CALL writeSharedLibComponentInfo(
    OUString const & rLibName, OUString const & rPath,
    Reference< lang::XMultiServiceFactory > const & xMgr,
    Reference< registry::XRegistryKey > const & xKey )
    SAL_THROW( (registry::CannotRegisterImplementationException) )
{
    OUString aModulePath( makeComponentPath( rLibName, rPath ) );

    if (! checkAccessPath( &aModulePath ))
    {
        throw registry::CannotRegisterImplementationException(
            OUSTR("permission denied to load component library: ") +
            aModulePath,
            Reference< XInterface >() );
    }
    
    oslModule lib = osl_loadModule(
        aModulePath.pData, SAL_LOADMODULE_LAZY | SAL_LOADMODULE_GLOBAL );
    if (! lib)
    {
        throw registry::CannotRegisterImplementationException(
            OUSTR("loading component library failed: ") + aModulePath,
            Reference< XInterface >() );
    }

    sal_Bool bRet = sal_False;

    uno::Environment currentEnv(Environment::getCurrent());
    uno::Environment env;
    
    OUString aEnvTypeName;
    OUString aExcMsg = getLibEnv(aModulePath, lib, &env, &aEnvTypeName, currentEnv);
    if (!aExcMsg.getLength())
    {
        OUString aWriteInfoName = OUSTR(COMPONENT_WRITEINFO);
        oslGenericFunction pSym = osl_getFunctionSymbol( lib, aWriteInfoName.pData );
        if (pSym != 0)
        {
            if (!env.is())
                env = uno::Environment(aEnvTypeName);
            
            if (env.is() && currentEnv.is())
            {
                Mapping aCurrent2Env( currentEnv, env );
                if (aCurrent2Env.is())
                {
                    void * pSMgr = aCurrent2Env.mapInterface(
                        xMgr.get(), ::getCppuType( &xMgr ) );
                    void * pKey = aCurrent2Env.mapInterface(
                        xKey.get(), ::getCppuType( &xKey ) );
                    if (pKey)
                    {
                        env.invoke(s_writeInfo, pSym, pSMgr, pKey, &bRet);


                        (*env.get()->pExtEnv->releaseInterface)(
                            env.get()->pExtEnv, pKey );
                        if (! bRet)
                        {
                            aExcMsg = aModulePath;
                            aExcMsg += OUSTR(": component_writeInfo() "
                                             "returned false!");
                        }
                    }
                    else
                    {
                        // key is mandatory
                        aExcMsg = aModulePath;
                        aExcMsg += OUSTR(": registry is mandatory to invoke"
                                         " component_writeInfo()!");
                    }
                    
                    if (pSMgr)
                    {
                        (*env.get()->pExtEnv->releaseInterface)(
                            env.get()->pExtEnv, pSMgr );
                    }
                }
                else
                {
                    aExcMsg = OUSTR("cannot get uno mapping: C++ <=> UNO!");
                }
            }
            else
            {
                aExcMsg = OUSTR("cannot get uno environments!");
            }
        }
        else
        {
            aExcMsg = aModulePath;
            aExcMsg += OUSTR(": cannot get symbol: ");
            aExcMsg += aWriteInfoName;
        }
    }

//!
//! OK: please look at #88219#
//!
//! ::osl_unloadModule( lib);
    if (! bRet)
    {
#if OSL_DEBUG_LEVEL > 1
        out( "### cannot write component info: " );
        out( aExcMsg );
        out( "\n" );
#endif
        throw registry::CannotRegisterImplementationException(
            aExcMsg, Reference< XInterface >() );
    }
}

}
