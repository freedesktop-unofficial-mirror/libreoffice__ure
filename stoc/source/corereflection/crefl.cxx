/*************************************************************************
 *
 *  $RCSfile: crefl.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: vg $ $Date: 2003-04-15 17:12:46 $
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

#ifndef _CPPUHELPER_QUERYINTERFACE_HXX_
#include <cppuhelper/queryinterface.hxx>
#endif
#ifndef _CPPUHELPER_IMPLEMENTATIONENTRY_HXX_
#include <cppuhelper/implementationentry.hxx>
#endif

#include <com/sun/star/registry/XRegistryKey.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/reflection/XTypeDescription.hpp>

using namespace com::sun::star;
using namespace com::sun::star::lang;
using namespace com::sun::star::registry;
using namespace cppu;
using namespace osl;
using namespace rtl;

#include "base.hxx"


namespace stoc_corefl
{

static const sal_Int32 CACHE_SIZE = 256;

#define SERVICENAME "com.sun.star.reflection.CoreReflection"
#define IMPLNAME	"com.sun.star.comp.stoc.CoreReflection"

// can be static, as every client of the core reflection keeps a reference to the
// core reflection, so refcounting can be done here.
static rtl_StandardModuleCount g_moduleCount = MODULE_COUNT_INIT;

static Sequence< OUString > core_getSupportedServiceNames()
{
    static Sequence < OUString > *pNames = 0;
    if( ! pNames )
    {
        MutexGuard guard( Mutex::getGlobalMutex() );
        if( !pNames )
        {
            static Sequence< OUString > seqNames(1);
            seqNames.getArray()[0] = OUString( RTL_CONSTASCII_USTRINGPARAM(SERVICENAME) );
            pNames = &seqNames;
        }
    }
    return *pNames;
}

static OUString core_getImplementationName()
{
    static OUString *pImplName = 0;
    if( ! pImplName )
    {
        MutexGuard guard( Mutex::getGlobalMutex() );
        if( ! pImplName )
        {
            static OUString implName( RTL_CONSTASCII_USTRINGPARAM( IMPLNAME ) );
            pImplName = &implName;
        }
    }
    return *pImplName;
}
//__________________________________________________________________________________________________
IdlReflectionServiceImpl::IdlReflectionServiceImpl(
    const Reference< XComponentContext > & xContext )
    : OComponentHelper( _aComponentMutex )
    , _xMgr( xContext->getServiceManager(), UNO_QUERY )
    , _aElements( CACHE_SIZE )
{
    g_moduleCount.modCnt.acquire( &g_moduleCount.modCnt );
    xContext->getValueByName( OUString( RTL_CONSTASCII_USTRINGPARAM(
        "/singletons/com.sun.star.reflection.theTypeDescriptionManager") ) ) >>= _xTDMgr;
    OSL_ENSURE( _xTDMgr.is(), "### cannot get singleton \"TypeDescriptionManager\" from context!" );
}
//__________________________________________________________________________________________________
IdlReflectionServiceImpl::~IdlReflectionServiceImpl()
{
    TRACE( "> IdlReflectionServiceImpl dtor <\n" );
    g_moduleCount.modCnt.release( &g_moduleCount.modCnt );
}

// XInterface
//__________________________________________________________________________________________________
Any IdlReflectionServiceImpl::queryInterface( const Type & rType )
    throw(::com::sun::star::uno::RuntimeException)
{
    Any aRet( ::cppu::queryInterface(
        rType,
        static_cast< XIdlReflection * >( this ),
        static_cast< XHierarchicalNameAccess * >( this ),
        static_cast< XServiceInfo * >( this ) ) );

    return (aRet.hasValue() ? aRet : OComponentHelper::queryInterface( rType ));
}
//__________________________________________________________________________________________________
void IdlReflectionServiceImpl::acquire() throw()
{
    OComponentHelper::acquire();
}
//__________________________________________________________________________________________________
void IdlReflectionServiceImpl::release() throw()
{
    OComponentHelper::release();
}

// XTypeProvider
//__________________________________________________________________________________________________
Sequence< Type > IdlReflectionServiceImpl::getTypes()
    throw (::com::sun::star::uno::RuntimeException)
{
    static OTypeCollection * s_pTypes = 0;
    if (! s_pTypes)
    {
        MutexGuard aGuard( _aComponentMutex );
        if (! s_pTypes)
        {
            static OTypeCollection s_aTypes(
                ::getCppuType( (const Reference< XIdlReflection > *)0 ),
                ::getCppuType( (const Reference< XHierarchicalNameAccess > *)0 ),
                ::getCppuType( (const Reference< XServiceInfo > *)0 ),
                OComponentHelper::getTypes() );
            s_pTypes = &s_aTypes;
        }
    }
    return s_pTypes->getTypes();
}
//__________________________________________________________________________________________________
Sequence< sal_Int8 > IdlReflectionServiceImpl::getImplementationId()
    throw (::com::sun::star::uno::RuntimeException)
{
    static OImplementationId * s_pId = 0;
    if (! s_pId)
    {
        MutexGuard aGuard( _aComponentMutex );
        if (! s_pId)
        {
            static OImplementationId s_aId;
            s_pId = &s_aId;
        }
    }
    return s_pId->getImplementationId();
}

// XComponent
//__________________________________________________________________________________________________
void IdlReflectionServiceImpl::dispose()
    throw(::com::sun::star::uno::RuntimeException)
{
    TRACE( "> disposing corereflection... <" );
    OComponentHelper::dispose();

    MutexGuard aGuard( _aComponentMutex );
    _aElements.clear();
    _xTDMgr.clear();
    _xMgr.clear();
#ifdef TEST_LIST_CLASSES
    OSL_ENSURE( g_aClassNames.size() == 0, "### idl classes still alive!" );
    ClassNameList::const_iterator iPos( g_aClassNames.begin() );
    while (iPos != g_aClassNames.end())
    {
        OUString aName( *iPos );
        ++iPos;
    }
#endif
}

// XServiceInfo
//__________________________________________________________________________________________________
OUString IdlReflectionServiceImpl::getImplementationName()
    throw(::com::sun::star::uno::RuntimeException)
{
    return core_getImplementationName();
}
//__________________________________________________________________________________________________
sal_Bool IdlReflectionServiceImpl::supportsService( const OUString & rServiceName )
    throw(::com::sun::star::uno::RuntimeException)
{
    const Sequence< OUString > & rSNL = getSupportedServiceNames();
    const OUString * pArray = rSNL.getConstArray();
    for ( sal_Int32 nPos = rSNL.getLength(); nPos--; )
    {
        if (pArray[nPos] == rServiceName)
            return sal_True;
    }
    return sal_False;
}
//__________________________________________________________________________________________________
Sequence< OUString > IdlReflectionServiceImpl::getSupportedServiceNames()
    throw(::com::sun::star::uno::RuntimeException)
{
    return core_getSupportedServiceNames();
}

// XIdlReflection
//__________________________________________________________________________________________________
Reference< XIdlClass > IdlReflectionServiceImpl::getType( const Any & rObj )
    throw(::com::sun::star::uno::RuntimeException)
{
    return (rObj.hasValue() ? forType( rObj.getValueTypeRef() ) : Reference< XIdlClass >());
}

//__________________________________________________________________________________________________
inline Reference< XIdlClass > IdlReflectionServiceImpl::constructClass(
    typelib_TypeDescription * pTypeDescr )
{
    OSL_ENSURE( pTypeDescr->eTypeClass != typelib_TypeClass_TYPEDEF, "### unexpected typedef!" );

    switch (pTypeDescr->eTypeClass)
    {
    case typelib_TypeClass_VOID:
    case typelib_TypeClass_CHAR:
    case typelib_TypeClass_BOOLEAN:
    case typelib_TypeClass_BYTE:
    case typelib_TypeClass_SHORT:
    case typelib_TypeClass_UNSIGNED_SHORT:
    case typelib_TypeClass_LONG:
    case typelib_TypeClass_UNSIGNED_LONG:
    case typelib_TypeClass_HYPER:
    case typelib_TypeClass_UNSIGNED_HYPER:
    case typelib_TypeClass_FLOAT:
    case typelib_TypeClass_DOUBLE:
    case typelib_TypeClass_STRING:
    case typelib_TypeClass_ANY:
        return new IdlClassImpl( this, pTypeDescr->pTypeName, pTypeDescr->eTypeClass, pTypeDescr );

    case TypeClass_ENUM:
        return new EnumIdlClassImpl( this, pTypeDescr->pTypeName, pTypeDescr->eTypeClass, pTypeDescr );

    case typelib_TypeClass_STRUCT:
    case typelib_TypeClass_UNION:
    case typelib_TypeClass_EXCEPTION:
        return new CompoundIdlClassImpl( this, pTypeDescr->pTypeName, pTypeDescr->eTypeClass, pTypeDescr );

    case typelib_TypeClass_ARRAY:
    case typelib_TypeClass_SEQUENCE:
        return new ArrayIdlClassImpl( this, pTypeDescr->pTypeName, pTypeDescr->eTypeClass, pTypeDescr );

    case typelib_TypeClass_INTERFACE:
        return new InterfaceIdlClassImpl( this, pTypeDescr->pTypeName, pTypeDescr->eTypeClass, pTypeDescr );

    case typelib_TypeClass_TYPE:
        return new IdlClassImpl( this, pTypeDescr->pTypeName, pTypeDescr->eTypeClass, pTypeDescr );
#if OSL_DEBUG_LEVEL > 1
    case typelib_TypeClass_INTERFACE_METHOD:
    case typelib_TypeClass_INTERFACE_ATTRIBUTE:
    case typelib_TypeClass_SERVICE:
    case typelib_TypeClass_MODULE:
    case typelib_TypeClass_UNKNOWN:
    case typelib_TypeClass_TYPEDEF:
    default:
        OSL_TRACE( "### corereflection type unsupported: " );
        OString aName( OUStringToOString( pTypeDescr->pTypeName, RTL_TEXTENCODING_ASCII_US ) );
        OSL_TRACE( aName.getStr() );
        OSL_TRACE( "\n" );
#endif
    }
    return Reference< XIdlClass >();
}
//__________________________________________________________________________________________________
Reference< XIdlClass > IdlReflectionServiceImpl::forName( const OUString & rTypeName )
    throw(::com::sun::star::uno::RuntimeException)
{
    Reference< XIdlClass > xRet;
    Any aAny( _aElements.getValue( rTypeName ) );

    if (aAny.hasValue())
    {
        if (aAny.getValueTypeClass() == TypeClass_INTERFACE)
            xRet = *(const Reference< XIdlClass > *)aAny.getValue();
    }
    else
    {
        // try to get _type_ by name
        typelib_TypeDescription * pTD = 0;
        typelib_typedescription_getByName( &pTD, rTypeName.pData );
        if (pTD)
        {
            if ((xRet = constructClass( pTD )).is())
                _aElements.setValue( rTypeName, makeAny( xRet ) ); // update
            typelib_typedescription_release( pTD );
        }
    }

    return xRet;
}

// XHierarchicalNameAccess
//__________________________________________________________________________________________________
Any IdlReflectionServiceImpl::getByHierarchicalName( const OUString & rName )
    throw(::com::sun::star::container::NoSuchElementException, ::com::sun::star::uno::RuntimeException)
{
    Any aRet( _aElements.getValue( rName ) );
    if (! aRet.hasValue())
    {
        // first look for constants exclusivly!
        aRet = _xTDMgr->getByHierarchicalName( rName );
        if (aRet.getValueTypeClass() == TypeClass_INTERFACE) // if no constant,
                                                             // i.e. XTypeDescription for a type
        {
            // type retrieved from tdmgr
            OSL_ASSERT( (*(Reference< XInterface > *)aRet.getValue())->queryInterface(
                ::getCppuType( (const Reference< XTypeDescription > *)0 ) ).hasValue() );

            // if you are interested in a type then CALL forName()!!!
            // this way is NOT recommended for types, because this method looks for constants first

            // if td manager found some type, it will be in the cache (hopefully.. we just got it)
            // so the second retrieving via c typelib callback chain should succeed...

            // try to get _type_ by name
            typelib_TypeDescription * pTD = 0;
            typelib_typedescription_getByName( &pTD, rName.pData );

            aRet.clear(); // kick XTypeDescription interface

            if (pTD)
            {
                Reference< XIdlClass > xIdlClass( constructClass( pTD ) );
                aRet.setValue( &xIdlClass, ::getCppuType( (const Reference< XIdlClass > *)0 ) );
                typelib_typedescription_release( pTD );
            }
        }
        // else is constant

        // update
        if (aRet.hasValue())
            _aElements.setValue( rName, aRet );
        else
        {
            throw NoSuchElementException( rName, Reference< XInterface >() );
        }
    }
    return aRet;
}
//__________________________________________________________________________________________________
sal_Bool IdlReflectionServiceImpl::hasByHierarchicalName( const OUString & rName )
    throw(::com::sun::star::uno::RuntimeException)
{
    try
    {
        return getByHierarchicalName( rName ).hasValue();
    }
    catch (NoSuchElementException &)
    {
    }
    return sal_False;
}

//__________________________________________________________________________________________________
Reference< XIdlClass > IdlReflectionServiceImpl::forType( typelib_TypeDescription * pTypeDescr )
    throw(::com::sun::star::uno::RuntimeException)
{
    Reference< XIdlClass > xRet;
    OUString aName( pTypeDescr->pTypeName );
    Any aAny( _aElements.getValue( aName ) );

    if (aAny.hasValue())
    {
        if (aAny.getValueTypeClass() == TypeClass_INTERFACE)
            xRet = *(const Reference< XIdlClass > *)aAny.getValue();
    }
    else
    {
        if (pTypeDescr && (xRet = constructClass( pTypeDescr )).is())
            _aElements.setValue( aName, makeAny( xRet ) ); // update
    }

    return xRet;
}
//__________________________________________________________________________________________________
Reference< XIdlClass > IdlReflectionServiceImpl::forType( typelib_TypeDescriptionReference * pRef )
    throw(::com::sun::star::uno::RuntimeException)
{
    typelib_TypeDescription * pTD = 0;
    TYPELIB_DANGER_GET( &pTD, pRef );
    if (pTD)
    {
        Reference< XIdlClass > xRet = forType( pTD );
        TYPELIB_DANGER_RELEASE( pTD );
        return xRet;
    }
    throw RuntimeException(
        OUString( RTL_CONSTASCII_USTRINGPARAM("IdlReflectionServiceImpl::forType() failed!") ),
        (XWeak *)(OWeakObject *)this );
    return Reference< XIdlClass >(); // dummy
}

//__________________________________________________________________________________________________
const Mapping & IdlReflectionServiceImpl::getCpp2Uno()
    throw(::com::sun::star::uno::RuntimeException)
{
    if (! _aCpp2Uno.is())
    {
        MutexGuard aGuard( getMutexAccess() );
        if (! _aCpp2Uno.is())
        {
            _aCpp2Uno = Mapping(
                OUString( RTL_CONSTASCII_USTRINGPARAM(CPPU_CURRENT_LANGUAGE_BINDING_NAME) ),
                OUString( RTL_CONSTASCII_USTRINGPARAM(UNO_LB_UNO) ) );
            OSL_ENSURE( _aCpp2Uno.is(), "### cannot get c++ to uno mapping!" );
            if (! _aCpp2Uno.is())
            {
                throw RuntimeException(
                    OUString( RTL_CONSTASCII_USTRINGPARAM("cannot get c++ to uno mapping!") ),
                    (XWeak *)(OWeakObject *)this );
            }
        }
    }
    return _aCpp2Uno;
}
//__________________________________________________________________________________________________
const Mapping & IdlReflectionServiceImpl::getUno2Cpp()
    throw(::com::sun::star::uno::RuntimeException)
{
    if (! _aUno2Cpp.is())
    {
        MutexGuard aGuard( getMutexAccess() );
        if (! _aUno2Cpp.is())
        {
            _aUno2Cpp = Mapping(
                OUString( RTL_CONSTASCII_USTRINGPARAM(UNO_LB_UNO) ),
                OUString( RTL_CONSTASCII_USTRINGPARAM(CPPU_CURRENT_LANGUAGE_BINDING_NAME) ) );
            OSL_ENSURE( _aUno2Cpp.is(), "### cannot get uno to c++ mapping!" );
            if (! _aUno2Cpp.is())
            {
                throw RuntimeException(
                    OUString( RTL_CONSTASCII_USTRINGPARAM("cannot get uno to c++ mapping!") ),
                    (XWeak *)(OWeakObject *)this );
            }
        }
    }
    return _aUno2Cpp;
}
//__________________________________________________________________________________________________
uno_Interface * IdlReflectionServiceImpl::mapToUno(
    const Any & rObj, typelib_InterfaceTypeDescription * pTo )
    throw(::com::sun::star::uno::RuntimeException)
{
    Reference< XInterface > xObj;
    if (extract( rObj, pTo, xObj, this ))
        return (uno_Interface *)getCpp2Uno().mapInterface( xObj.get(), pTo );

    throw RuntimeException(
        OUString( RTL_CONSTASCII_USTRINGPARAM("illegal object given!") ),
        (XWeak *)(OWeakObject *)this );
}

//==================================================================================================
Reference< XInterface > SAL_CALL IdlReflectionServiceImpl_create(
    const Reference< XComponentContext > & xContext )
    throw(::com::sun::star::uno::Exception)
{
    return Reference< XInterface >( (XWeak *)(OWeakObject *)new IdlReflectionServiceImpl( xContext ) );
}

}


//##################################################################################################
//##################################################################################################
//##################################################################################################

using namespace stoc_corefl;

static struct ImplementationEntry g_entries[] =
{
    {
        IdlReflectionServiceImpl_create, core_getImplementationName,
        core_getSupportedServiceNames, createSingleComponentFactory,
        &g_moduleCount.modCnt , 0
    },
    { 0, 0, 0, 0, 0, 0 }
};

extern "C"
{
sal_Bool SAL_CALL component_canUnload( TimeValue *pTime )
{
    return g_moduleCount.canUnload( &g_moduleCount , pTime );
}

//==================================================================================================
void SAL_CALL component_getImplementationEnvironment(
    const sal_Char ** ppEnvTypeName, uno_Environment ** ppEnv )
{
    *ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}
//==================================================================================================
sal_Bool SAL_CALL component_writeInfo(
    void * pServiceManager, void * pRegistryKey )
{
    if (component_writeInfoHelper( pServiceManager, pRegistryKey, g_entries ))
    {
        try
        {
            // register singleton
            registry::XRegistryKey * pKey =
                reinterpret_cast< registry::XRegistryKey * >( pRegistryKey );
            Reference< registry::XRegistryKey > xKey(
                pKey->createKey(
                    OUSTR(IMPLNAME "/UNO/SINGLETONS/com.sun.star.reflection.theCoreReflection") ) );
            xKey->setStringValue( OUSTR("com.sun.star.reflection.CoreReflection") );
            return sal_True;
        }
        catch (Exception & exc)
        {
#if OSL_DEBUG_LEVEL > 0
            OString cstr( OUStringToOString( exc.Message, RTL_TEXTENCODING_ASCII_US ) );
            OSL_ENSURE( 0, cstr.getStr() );
#endif
        }
    }
    return sal_False;
}
//==================================================================================================
void * SAL_CALL component_getFactory(
    const sal_Char * pImplName, void * pServiceManager, void * pRegistryKey )
{
    return component_getFactoryHelper( pImplName, pServiceManager, pRegistryKey , g_entries );
}
}
