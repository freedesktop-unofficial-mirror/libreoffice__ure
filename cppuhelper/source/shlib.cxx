/*************************************************************************
 *
 *  $RCSfile: shlib.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hjs $ $Date: 2001-01-16 10:29:11 $
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

#ifdef DEBUG
#include <stdio.h>
#endif
#include <vector>

#include <osl/diagnose.h>
#include <osl/file.h>
#include <osl/mutex.hxx>
#include <osl/module.h>

#include <rtl/ustrbuf.hxx>

#include <uno/environment.h>
#include <uno/mapping.hxx>

#include <cppuhelper/factory.hxx>
#include <cppuhelper/shlib.hxx>

using namespace ::rtl;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::loader;
using namespace ::com::sun::star::registry;


namespace cppu
{

#ifdef DEBUG
//==================================================================================================
static inline void out( const char * p ) throw ()
{
    ::fprintf( stderr, p );
}
static inline void out( const OUString & r ) throw ()
{
    OString s( OUStringToOString( r, RTL_TEXTENCODING_ASCII_US ) );
    out( s.getStr() );
}
#endif

//==================================================================================================
static const ::std::vector< OUString > * getAccessDPath() throw ()
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
                sal_Int32 nToken = aEnv.getTokenCount( ';' );
                
                for ( sal_Int32 i = 0; i < nToken; ++i )
                {
                    OUString aStr( OStringToOUString(
                        aEnv.getToken( i, ';' ), RTL_TEXTENCODING_ASCII_US ) );
                    OUString aUNC;
                    if (osl_File_E_None == ::osl_normalizePath( aStr.pData, &aUNC.pData ))
                    {
                        s_v.push_back( aUNC );
                    }
                }
#ifdef DEBUG
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
            s_bInit = true;
        }
    }
    
    return s_p;
}

//==================================================================================================
static bool checkAccessPath( OUString * pComp ) throw ()
{
#ifdef DEBUG
    out( "> trying to access module: \"" );
    out( *pComp );
    out( "\"\n" );
#endif
    
    const ::std::vector< OUString > * pPath = getAccessDPath();
    
    if (pPath)
    {
        for ( ::std::vector< OUString >::const_iterator iPos( pPath->begin() );
              iPos != pPath->end(); ++iPos )
        {
            OUString aBaseDir( *iPos );
            OUString aAbs;
            
            if (pComp->getLength() > 2 &&
                pComp->getStr()[0] == (sal_Unicode)'/' &&
                pComp->getStr()[1] == (sal_Unicode)'/') // absolute unc path given?
            {
                aAbs = *pComp;
#ifdef DEBUG
                out( "> taking path: \"" );
                out( aAbs );
#endif
            }
            else
            {
                if (osl_File_E_None != ::osl_getAbsolutePath( aBaseDir.pData, pComp->pData, &aAbs.pData ))
                {
                    continue;
                }
#ifdef DEBUG
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
                 aAbs[ aBaseDir.getLength() ] == (sal_Unicode)'/')) // dir boundary
            {
#ifdef DEBUG
                out( ": ok.\n" );
#endif
                // load from absolute path
                *pComp = aAbs;
                return true;
            }
#ifdef DEBUG
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
#ifdef DEBUG
        out( "=> no CPLD_ACCESSPATH set.\n" );
#endif
        return true;
    }
}

//--------------------------------------------------------------------------------------------------
static inline sal_Int32 endsWith( const OUString & rText, const OUString & rEnd ) throw ()
{
    if (rText.getLength() >= rEnd.getLength() &&
        rEnd.equalsIgnoreCase( rText.copy( rText.getLength() - rEnd.getLength() ) ))
    {
        return rText.getLength() - rEnd.getLength();
    }
    return -1;
}
//==================================================================================================
static OUString makeComponentPath(
    const OUString & rLibName, const OUString & rPath
#ifdef MACOSX // only for mac hack: provide pure lib name
    , OUString * pCompName
#endif
    ) throw ()
{
    OUStringBuffer buf( rPath.getLength() + 32 );
    
    if (rPath.getLength())
    {
        buf.append( rPath );
#if defined( SAL_UNX ) || defined( MACOSX )
        if (rPath[ rPath.getLength() -1 ] != (sal_Unicode)'/')
        {
            buf.append( (sal_Unicode)'/' ); // for unc and system
        }
#else
        if (rPath.getLength() > 2 &&
            rPath[0] == (sal_Unicode)'/' && rPath[1] == (sal_Unicode)'/' && // unc path
            rPath[ rPath.getLength() -1 ] != '/')
        {
            buf.append( (sal_Unicode)'/' );
        }
        else if (rPath[ rPath.getLength() -1 ] != (sal_Unicode)'\\')
        {
            buf.append( (sal_Unicode)'\\' );
        }
#endif
    }
    
#if defined( MACOSX )
    // determine start and end of component name
    sal_Int32 nEnd = endsWith( rLibName, OUString( RTL_CONSTASCII_USTRINGPARAM(".dylib.framework") ) );
    if (nEnd < 0)
    {
        buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("lib") );
        buf.append( *pCompName = rLibName.copy( rLibName.lastIndexOf( (sal_Unicode)'/' ) +1 ) );
        buf.appendAscii( RTL_CONSTASCII_STRINGPARAM(".dylib.framework") );
    }
    else // name is completely pre/postfixed
    {
        sal_Int32 nStart = rLibName.lastIndexOf( (sal_Unicode)'/' ) +4;  // /lib...
        *pCompName = rLibName.copy( nStart, nEnd - nStart );
        buf.append( rLibName );
    }
#elif defined( SAL_UNX )
    sal_Int32 nEnd = endsWith( rLibName, OUString( RTL_CONSTASCII_USTRINGPARAM(".so") ) );
    if (nEnd < 0) // !endsWith
    {
        buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("lib") );
        buf.append( rLibName );
        buf.appendAscii( RTL_CONSTASCII_STRINGPARAM(".so") );
    }
    else // name is completely pre/postfixed
    {
        buf.append( rLibName );
    }
#elif defined( OS2 )
    buf.append( rLibName );
#else // windows
    buf.append( rLibName );
    sal_Int32 nEnd = endsWith( rLibName, OUString( RTL_CONSTASCII_USTRINGPARAM(".dll") ) );
    if (nEnd < 0) // !endsWith
    {
        buf.appendAscii( RTL_CONSTASCII_STRINGPARAM(".dll") );
    }
#endif
    
    return buf.makeStringAndClear();
}

//==================================================================================================
Reference< XSingleServiceFactory > SAL_CALL loadSharedLibComponentFactory(
    OUString const & rLibName, OUString const & rPath,
    OUString const & rImplName,
    Reference< XMultiServiceFactory > const & xMgr,
    Reference< XRegistryKey > const & xKey )
    throw (CannotActivateFactoryException)
{
#ifdef MACOSX
    OUString aComponentName;
    OUString aModulePath( makeComponentPath( rLibName, rPath, &aComponentName ) );
#else
    OUString aModulePath( makeComponentPath( rLibName, rPath ) );
#endif
    
    OUString aExcMsg;
    
    if (! checkAccessPath( &aModulePath ))
    {
        aExcMsg = OUString( RTL_CONSTASCII_USTRINGPARAM("permission denied to load component library: ") );
        aExcMsg += aModulePath;
        throw CannotActivateFactoryException( aExcMsg, Reference< XInterface >() );
    }
    
    oslModule lib = ::osl_loadModule(
        aModulePath.pData, SAL_LOADMODULE_LAZY | SAL_LOADMODULE_GLOBAL );
    if (! lib)
    {
        aExcMsg = OUString( RTL_CONSTASCII_USTRINGPARAM("loading component library failed: ") );
        aExcMsg += aModulePath;
        throw CannotActivateFactoryException( aExcMsg, Reference< XInterface >() );
    }
    
    Reference< XSingleServiceFactory > xRet;
    
    void * pSym;	
#ifdef MACOSX
    OUString aGetEnvName(
        aComponentName + OUString( RTL_CONSTASCII_USTRINGPARAM(COMPONENT_GETENV) ) );
#else
    OUString aGetEnvName( RTL_CONSTASCII_USTRINGPARAM(COMPONENT_GETENV) );
#endif
    if (pSym = ::osl_getSymbol( lib, aGetEnvName.pData ))
    {
        uno_Environment * pCurrentEnv = 0;
        uno_Environment * pEnv = 0;
        
        const sal_Char * pEnvTypeName = 0;
        (*((component_getImplementationEnvironmentFunc)pSym))( &pEnvTypeName, &pEnv );
        OUString aEnvTypeName( OUString::createFromAscii( pEnvTypeName ) );
        
        sal_Bool bNeedsMapping =
            (pEnv || aEnvTypeName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(CPPU_CURRENT_LANGUAGE_BINDING_NAME ) ));
        
        if (bNeedsMapping)
        {
            if (! pEnv)
            {
                ::uno_getEnvironment( &pEnv, aEnvTypeName.pData, 0 );
            }
            if (pEnv)
            {
                OUString aCppEnvTypeName( RTL_CONSTASCII_USTRINGPARAM(CPPU_CURRENT_LANGUAGE_BINDING_NAME) );
                ::uno_getEnvironment( &pCurrentEnv, aCppEnvTypeName.pData, 0 );
                if (pCurrentEnv)
                {
                    bNeedsMapping = (pEnv != pCurrentEnv);
                }
            }
        }
        
#ifdef MACOSX
        OUString aGetFactoryName(
            aComponentName + OUString( RTL_CONSTASCII_USTRINGPARAM(COMPONENT_GETFACTORY) ) );
#else
        OUString aGetFactoryName( RTL_CONSTASCII_USTRINGPARAM(COMPONENT_GETFACTORY) );
#endif
        if (pSym = ::osl_getSymbol( lib, aGetFactoryName.pData ))
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
                        void * pSMgr = aCurrent2Env.mapInterface( xMgr.get(), ::getCppuType( &xMgr ) );
                        void * pKey = aCurrent2Env.mapInterface( xKey.get(), ::getCppuType( &xKey ) );
                        
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
                                pSSF, ::getCppuType( &xRet ) );
                            (*pEnv->pExtEnv->releaseInterface)( pEnv->pExtEnv, pSSF );
                        }
                        else
                        {
                            aExcMsg = aModulePath;
                            aExcMsg += OUString( RTL_CONSTASCII_USTRINGPARAM(": cannot get factory of demanded implementation: ") );
                            aExcMsg += OStringToOUString( aImplName, RTL_TEXTENCODING_ASCII_US );
                        }
                    }
                    else
                    {
                        aExcMsg = OUString( RTL_CONSTASCII_USTRINGPARAM("cannot get uno mappings: C++ <=> UNO!") );
                    }
                }
                else
                {
                    aExcMsg = OUString( RTL_CONSTASCII_USTRINGPARAM("cannot get uno environments!") );
                }
            }
            else
            {
                XSingleServiceFactory * pRet = (XSingleServiceFactory *)
                    (*((component_getFactoryFunc)pSym))( aImplName.getStr(), xMgr.get(), xKey.get() );
                if (pRet)
                {
                    xRet = pRet;
                    pRet->release();
                }
                else
                {
                    aExcMsg = aModulePath;
                    aExcMsg += OUString( RTL_CONSTASCII_USTRINGPARAM(": cannot get factory of demanded implementation: ") );
                    aExcMsg += OStringToOUString( aImplName, RTL_TEXTENCODING_ASCII_US );
                }
            }
        }
        else
        {
            aExcMsg = aModulePath;
            aExcMsg += OUString( RTL_CONSTASCII_USTRINGPARAM(": cannot get symbol: ") );
            aExcMsg += aGetFactoryName;
        }
        
        if (pEnv)
            (*pEnv->release)( pEnv );
        if (pCurrentEnv)
            (*pCurrentEnv->release)( pCurrentEnv );
    }
    else
    {
        aExcMsg = aModulePath;
        aExcMsg += OUString( RTL_CONSTASCII_USTRINGPARAM(": cannot get symbol: ") );
        aExcMsg += aGetEnvName;
    }
    
    if (! xRet.is())
    {
        ::osl_unloadModule( lib );
#ifdef DEBUG
        out( aExcMsg );
#endif
        throw CannotActivateFactoryException( aExcMsg, Reference< XInterface >() );
    }
    
    return xRet;
}

//==================================================================================================
void SAL_CALL writeSharedLibComponentInfo(
    OUString const & rLibName, OUString const & rPath,
    Reference< XMultiServiceFactory > const & xMgr,
    Reference< XRegistryKey > const & xKey )
    throw (CannotRegisterImplementationException)
{
#ifdef MACOSX
    OUString aComponentName;
    OUString aModulePath( makeComponentPath( rLibName, rPath, &aComponentName ) );
#else
    OUString aModulePath( makeComponentPath( rLibName, rPath ) );
#endif
    
    OUString aExcMsg;
    
    if (! checkAccessPath( &aModulePath ))
    {
        aExcMsg = OUString( RTL_CONSTASCII_USTRINGPARAM("permission denied to load component library: ") );
        aExcMsg += aModulePath;
        throw CannotRegisterImplementationException( aExcMsg, Reference< XInterface >() );
    }
    
    oslModule lib = ::osl_loadModule(
        aModulePath.pData, SAL_LOADMODULE_LAZY | SAL_LOADMODULE_GLOBAL );
    if (! lib)
    {
        aExcMsg = OUString( RTL_CONSTASCII_USTRINGPARAM("loading component library failed: ") );
        aExcMsg += aModulePath;
        throw CannotRegisterImplementationException( aExcMsg, Reference< XInterface >() );
    }
    
    sal_Bool bRet = sal_False;
    
    void * pSym;	
#ifdef MACOSX
    OUString aGetEnvName(
        aComponentName + OUString( RTL_CONSTASCII_USTRINGPARAM(COMPONENT_GETENV) ) );
#else
    OUString aGetEnvName( RTL_CONSTASCII_USTRINGPARAM(COMPONENT_GETENV) );
#endif
    if (pSym = ::osl_getSymbol( lib, aGetEnvName.pData ))
    {
        uno_Environment * pCurrentEnv = 0;
        uno_Environment * pEnv = 0;
        
        const sal_Char * pEnvTypeName = 0;
        (*((component_getImplementationEnvironmentFunc)pSym))( &pEnvTypeName, &pEnv );
        OUString aEnvTypeName( OUString::createFromAscii( pEnvTypeName ) );
        
        sal_Bool bNeedsMapping =
            (pEnv || aEnvTypeName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(CPPU_CURRENT_LANGUAGE_BINDING_NAME ) ));
        
        if (bNeedsMapping)
        {
            if (! pEnv)
            {
                ::uno_getEnvironment( &pEnv, aEnvTypeName.pData, 0 );
            }
            if (pEnv)
            {
                OUString aCppEnvTypeName( RTL_CONSTASCII_USTRINGPARAM(CPPU_CURRENT_LANGUAGE_BINDING_NAME) );
                ::uno_getEnvironment( &pCurrentEnv, aCppEnvTypeName.pData, 0 );
                if (pCurrentEnv)
                {
                    bNeedsMapping = (pEnv != pCurrentEnv);
                }
            }
        }
        
#ifdef MACOSX
        OUString aWriteInfoName(
            aComponentName + OUString( RTL_CONSTASCII_USTRINGPARAM(COMPONENT_WRITEINFO) ) );
#else
        OUString aWriteInfoName( RTL_CONSTASCII_USTRINGPARAM(COMPONENT_WRITEINFO) );
#endif
        if (pSym = ::osl_getSymbol( lib, aWriteInfoName.pData ))
        {
            if (bNeedsMapping)
            {
                if (pEnv && pCurrentEnv)
                {
                    Mapping aCurrent2Env( pCurrentEnv, pEnv );
                    if (aCurrent2Env.is())
                    {
                        void * pSMgr = aCurrent2Env.mapInterface( xMgr.get(), ::getCppuType( &xMgr ) );
                        void * pKey = aCurrent2Env.mapInterface( xKey.get(), ::getCppuType( &xKey ) );
                        if (pKey)
                        {
                            bRet = (*((component_writeInfoFunc)pSym))( pSMgr, pKey );
                            (*pEnv->pExtEnv->releaseInterface)( pEnv->pExtEnv, pKey );
                            if (! bRet)
                            {
                                aExcMsg = aModulePath;
                                aExcMsg += OUString( RTL_CONSTASCII_USTRINGPARAM(": component_writeInfo() returned false!") );
                            }
                        }
                        else
                        {
                            // key is mandatory
                            aExcMsg = aModulePath;
                            aExcMsg += OUString( RTL_CONSTASCII_USTRINGPARAM(": registry is mandatory to invoke component_writeInfo()!") );
                        }
                            
                        if (pSMgr)
                            (*pEnv->pExtEnv->releaseInterface)( pEnv->pExtEnv, pSMgr );
                    }
                    else
                    {
                        aExcMsg = OUString( RTL_CONSTASCII_USTRINGPARAM("cannot get uno mapping: C++ <=> UNO!") );
                    }
                }
                else
                {
                    aExcMsg = OUString( RTL_CONSTASCII_USTRINGPARAM("cannot get uno environments!") );
                }
            }
            else
            {
                if (xKey.is())
                {
                    bRet = (*((component_writeInfoFunc)pSym))( xMgr.get(), xKey.get() );
                    if (! bRet)
                    {
                        aExcMsg = aModulePath;
                        aExcMsg += OUString( RTL_CONSTASCII_USTRINGPARAM(": component_writeInfo() returned false!") );
                    }
                }
                else
                {
                    // key is mandatory
                    aExcMsg = aModulePath;
                    aExcMsg += OUString( RTL_CONSTASCII_USTRINGPARAM(": registry is mandatory to invoke component_writeInfo()!") );
                }
            }
        }
        else
        {
            aExcMsg = aModulePath;
            aExcMsg += OUString( RTL_CONSTASCII_USTRINGPARAM(": cannot get symbol: ") );
            aExcMsg += aWriteInfoName;
        }
        
        if (pEnv)
            (*pEnv->release)( pEnv );
        if (pCurrentEnv)
            (*pCurrentEnv->release)( pCurrentEnv );
    }
    else
    {
        aExcMsg = aModulePath;
        aExcMsg += OUString( RTL_CONSTASCII_USTRINGPARAM(": cannot get symbol: ") );
        aExcMsg += aGetEnvName;
    }
    
    if (! bRet)
    {
        ::osl_unloadModule( lib );
#ifdef DEBUG
        out( aExcMsg );
#endif
        throw CannotRegisterImplementationException( aExcMsg, Reference< XInterface >() );
    }
}

}
