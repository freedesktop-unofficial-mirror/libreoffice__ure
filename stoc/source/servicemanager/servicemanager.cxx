/*************************************************************************
 *
 *  $RCSfile: servicemanager.cxx,v $
 *
 *  $Revision: 1.23 $
 *
 *  last change: $Author: hr $ $Date: 2004-04-13 12:21:26 $
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

#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif
#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

#include <hash_map>
#include <hash_set>
#include <list>

#ifndef _UNO_MAPPING_HXX_
#include <uno/mapping.hxx>
#endif
#ifndef _UNO_DISPATCHER_H_
#include <uno/dispatcher.h>
#endif
#ifndef _CPPUHELPER_QUERYINTERFACE_HXX_
#include <cppuhelper/queryinterface.hxx>
#endif
#ifndef _CPPUHELPER_WEAKREF_HXX_
#include <cppuhelper/weakref.hxx>
#endif
#ifndef _CPPUHELPER_COMPONENT_HXX_
#include <cppuhelper/component.hxx>
#endif
#ifndef _CPPUHELPER_FACTORY_HXX_
#include <cppuhelper/factory.hxx>
#endif
#ifndef _CPPUHELPER_IMPLBASE1_HXX
#include <cppuhelper/implbase1.hxx>
#endif
#ifndef _CPPUHELPER_TYPEPROVIDER_HXX_
#include <cppuhelper/typeprovider.hxx>
#endif
#ifndef _CPPUHELPER_IMPLEMENTATIONENTRY_HXX_
#include <cppuhelper/implementationentry.hxx>
#endif
#ifndef _RTL_UNLOAD_H_
#include <rtl/unload.h>
#endif
#include <cppuhelper/component_context.hxx>
#include <cppuhelper/bootstrap.hxx>
#include <cppuhelper/compbase8.hxx>


#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XEventListener.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/registry/XRegistryKey.hpp>
#include <com/sun/star/registry/XSimpleRegistry.hpp>
#include <com/sun/star/container/XSet.hpp>
#include <com/sun/star/container/XElementAccess.hpp>
#include <com/sun/star/container/XEnumeration.hpp>
#include <com/sun/star/container/XContentEnumerationAccess.hpp>
#include <com/sun/star/container/XHierarchicalNameAccess.hpp>
#include <com/sun/star/uno/XUnloadingPreference.hpp>

#define OUSTR(x) ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(x) )


using namespace com::sun::star;
using namespace com::sun::star::uno;
using namespace com::sun::star::beans;
using namespace com::sun::star::registry;
using namespace com::sun::star::lang;
using namespace com::sun::star::container;
using namespace cppu;
using namespace osl;
using namespace rtl;
using namespace std;

namespace stoc_smgr
{

static rtl_StandardModuleCount g_moduleCount = MODULE_COUNT_INIT;

static Sequence< sal_Int8 > smgr_getImplementationId()
{
    static OImplementationId * s_pId = 0;
    if (! s_pId)
    {
        MutexGuard aGuard( Mutex::getGlobalMutex() );
        if (! s_pId)
        {
            static OImplementationId s_aId;
            s_pId = &s_aId;
        }
    }
    return s_pId->getImplementationId();
}

static Sequence< OUString > smgr_wrapper_getSupportedServiceNames()
{
    static Sequence < OUString > *pNames = 0;
    if( ! pNames )
    {
        MutexGuard guard( Mutex::getGlobalMutex() );
        if( !pNames )
        {
            static Sequence< OUString > seqNames(1);
            seqNames.getArray()[0] = OUString(
                RTL_CONSTASCII_USTRINGPARAM("com.sun.star.lang.MultiServiceFactory") );
            pNames = &seqNames;
        }
    }
    return *pNames;
}

static OUString smgr_wrapper_getImplementationName()
{
    static OUString *pImplName = 0;
    if( ! pImplName )
    {
        MutexGuard guard( Mutex::getGlobalMutex() );
        if( ! pImplName )
        {
            static OUString implName(
                RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.stoc.OServiceManagerWrapper" ) );
            pImplName = &implName;
        }
    }
    return *pImplName;
}

static Sequence< OUString > smgr_getSupportedServiceNames()
{
    static Sequence < OUString > *pNames = 0;
    if( ! pNames )
    {
        MutexGuard guard( Mutex::getGlobalMutex() );
        if( !pNames )
        {
            static Sequence< OUString > seqNames(2);
            seqNames.getArray()[0] = OUString(
                RTL_CONSTASCII_USTRINGPARAM("com.sun.star.lang.MultiServiceFactory") );
            seqNames.getArray()[1] = OUString(
                RTL_CONSTASCII_USTRINGPARAM("com.sun.star.lang.ServiceManager") );
            pNames = &seqNames;
        }
    }
    return *pNames;
}

static OUString smgr_getImplementationName()
{
    static OUString *pImplName = 0;
    if( ! pImplName )
    {
        MutexGuard guard( Mutex::getGlobalMutex() );
        if( ! pImplName )
        {
            static OUString implName(
                RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.stoc.OServiceManager" ) );
            pImplName = &implName;
        }
    }
    return *pImplName;
}

static Sequence< OUString > regsmgr_getSupportedServiceNames()
{
    static Sequence < OUString > *pNames = 0;
    if( ! pNames )
    {
        MutexGuard guard( Mutex::getGlobalMutex() );
        if( !pNames )
        {
            static Sequence< OUString > seqNames(2);
            seqNames.getArray()[0] = OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.lang.MultiServiceFactory"));
            seqNames.getArray()[1] = OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.lang.RegistryServiceManager"));
            pNames = &seqNames;
        }
    }
    return *pNames;
}

static OUString regsmgr_getImplementationName()
{
    static OUString *pImplName = 0;
    if( ! pImplName )
    {
        MutexGuard guard( Mutex::getGlobalMutex() );
        if( ! pImplName )
        {
            static OUString implName( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.stoc.ORegistryServiceManager" ) );
            pImplName = &implName;
        }
    }
    return *pImplName;
}

static Sequence< OUString > retrieveAsciiValueList(
    const Reference< XSimpleRegistry > &xReg, const OUString &keyName )
{
    Reference< XEnumerationAccess > xAccess( xReg, UNO_QUERY );
    Sequence< OUString > seq;
    if( xAccess.is() )
    {
        Reference< XEnumeration > xEnum = xAccess->createEnumeration();
        while( xEnum.is() && xEnum->hasMoreElements() )
        {
            Reference< XSimpleRegistry > xTempReg;
            xEnum->nextElement() >>= xTempReg;
            if( xTempReg.is() )
            {
                Sequence< OUString > seq2 = retrieveAsciiValueList( xTempReg, keyName );

                if( seq2.getLength() )
                {
                    sal_Int32 n1Len = seq.getLength();
                    sal_Int32 n2Len = seq2.getLength();

                    seq.realloc( n1Len + n2Len );
                    const OUString *pSource = seq2.getConstArray();
                    OUString *pTarget = seq.getArray();
                    for( int i = 0 ; i < n2Len ; i ++ )
                    {
                        pTarget[i+n1Len] = pSource[i];
                    }
                }
            }
        }
    }
    else if( xReg.is () )
    {
        try
        {
            Reference< XRegistryKey > rRootKey = xReg->getRootKey();
            if( rRootKey.is() )
            {
                Reference<XRegistryKey > xKey = rRootKey->openKey(keyName);
                if( xKey.is() )
                {
                    seq = xKey->getAsciiListValue();
                }
            }
        }
        catch( InvalidRegistryException & )
        {
        }
        catch (InvalidValueException &)
        {
        }
    }
    return seq;
}


/*****************************************************************************
    helper functions
*****************************************************************************/
OUString Point2Slash(const OUString& s)
{
    OUStringBuffer ret;

    sal_Int32 nIndex = 0;
    do
    {
        OUString token( s.getToken(0, '.', nIndex) );

        if (token.getLength())
        {
            ret.append( (sal_Unicode)'/' );
            ret.append( token );
        }
    } while( nIndex != -1 );

    return ret.makeStringAndClear();
}

/*****************************************************************************
    Enumeration by ServiceName
*****************************************************************************/
struct hashRef_Impl
{
    size_t operator()(const Reference<XInterface > & rName) const
    {
        // query to XInterface. The cast to XInterface* must be the same for the same object
        Reference<XInterface > x( Reference<XInterface >::query( rName ) );
        return (size_t)x.get();
    }
};

struct equaltoRef_Impl
{
    size_t operator()(const Reference<XInterface > & rName1, const Reference<XInterface > & rName2 ) const
        { return rName1 == rName2; }
};

typedef hash_set
<
    Reference<XInterface >,
    hashRef_Impl,
    equaltoRef_Impl
> HashSet_Ref;


class ServiceEnumeration_Impl : public WeakImplHelper1< XEnumeration >
{
public:
    ServiceEnumeration_Impl( const Sequence< Reference<XInterface > > & rFactories )
        : aFactories( rFactories )
        , nIt( 0 )
        { g_moduleCount.modCnt.acquire( &g_moduleCount.modCnt ); }
    virtual ~ServiceEnumeration_Impl()
        { g_moduleCount.modCnt.release( &g_moduleCount.modCnt ); }

    // XEnumeration
    sal_Bool SAL_CALL hasMoreElements()
        throw(::com::sun::star::uno::RuntimeException);
    Any SAL_CALL nextElement()
        throw(::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
private:
    Mutex								aMutex;
    Sequence< Reference<XInterface > >	aFactories;
    sal_Int32							nIt;
};

// XEnumeration
sal_Bool ServiceEnumeration_Impl::hasMoreElements() throw(::com::sun::star::uno::RuntimeException)
{
    MutexGuard aGuard( aMutex );
    return nIt != aFactories.getLength();
}

// XEnumeration
Any ServiceEnumeration_Impl::nextElement()
    throw(::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
{
    MutexGuard aGuard( aMutex );
    if( nIt == aFactories.getLength() )
        throw NoSuchElementException();

    return Any( &aFactories.getConstArray()[nIt++], ::getCppuType( (const Reference<XInterface > *)0 ) );
}

//==================================================================================================
class PropertySetInfo_Impl : public WeakImplHelper1< beans::XPropertySetInfo >
{
    Sequence< beans::Property > m_properties;

public:
    inline PropertySetInfo_Impl( Sequence< beans::Property > const & properties ) SAL_THROW( () )
        : m_properties( properties )
        {}

    // XPropertySetInfo impl
    virtual Sequence< beans::Property > SAL_CALL getProperties()
        throw (RuntimeException);
    virtual beans::Property SAL_CALL getPropertyByName( OUString const & name )
        throw (beans::UnknownPropertyException, RuntimeException);
    virtual sal_Bool SAL_CALL hasPropertyByName( OUString const & name )
        throw (RuntimeException);
};
//__________________________________________________________________________________________________
Sequence< beans::Property > PropertySetInfo_Impl::getProperties()
    throw (RuntimeException)
{
    return m_properties;
}
//__________________________________________________________________________________________________
beans::Property PropertySetInfo_Impl::getPropertyByName( OUString const & name )
    throw (beans::UnknownPropertyException, RuntimeException)
{
    beans::Property const * p = m_properties.getConstArray();
    for ( sal_Int32 nPos = m_properties.getLength(); nPos--; )
    {
        if (p[ nPos ].Name.equals( name ))
            return p[ nPos ];
    }
    throw beans::UnknownPropertyException(
        OUSTR("unknown property: ") + name, Reference< XInterface >() );
}
//__________________________________________________________________________________________________
sal_Bool PropertySetInfo_Impl::hasPropertyByName( OUString const & name )
    throw (RuntimeException)
{
    beans::Property const * p = m_properties.getConstArray();
    for ( sal_Int32 nPos = m_properties.getLength(); nPos--; )
    {
        if (p[ nPos ].Name.equals( name ))
            return sal_True;
    }
    return sal_False;
}


/*****************************************************************************
    Enumeration by implementation
*****************************************************************************/
class ImplementationEnumeration_Impl : public WeakImplHelper1< XEnumeration >
{
public:
    ImplementationEnumeration_Impl( const HashSet_Ref & rImplementationMap )
        : aImplementationMap( rImplementationMap )
        , aIt( aImplementationMap.begin() )
        {
            g_moduleCount.modCnt.acquire( &g_moduleCount.modCnt );
        }
    virtual ~ImplementationEnumeration_Impl();

    // XEnumeration
    virtual sal_Bool SAL_CALL hasMoreElements()
         throw(::com::sun::star::uno::RuntimeException);
    virtual Any SAL_CALL nextElement()
        throw(::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

private:
    Mutex							aMutex;
    HashSet_Ref						aImplementationMap;
    HashSet_Ref::iterator			aIt;
    sal_Int32						nNext;
    Reference<XInterface >			xNext;
};

ImplementationEnumeration_Impl::~ImplementationEnumeration_Impl()
{
    g_moduleCount.modCnt.release( &g_moduleCount.modCnt );
}

// XEnumeration
sal_Bool ImplementationEnumeration_Impl::hasMoreElements()
    throw(::com::sun::star::uno::RuntimeException)
{
    MutexGuard aGuard( aMutex );
    return aIt != aImplementationMap.end();
}

// XEnumeration
Any ImplementationEnumeration_Impl::nextElement()
    throw(::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
{
    MutexGuard aGuard( aMutex );
    if( aIt == aImplementationMap.end() )
        throw NoSuchElementException();

    Any ret( &(*aIt), ::getCppuType( (const Reference<XInterface > *)0 ) );
    ++aIt;
    return ret;
}

/*****************************************************************************
    Hash tables
*****************************************************************************/
struct equalOWString_Impl
{
  sal_Bool operator()(const OUString & s1, const OUString & s2) const
        { return s1 == s2; }
};

struct hashOWString_Impl
{
    size_t operator()(const OUString & rName) const
        { return rName.hashCode(); }
};

typedef hash_set
<
    OUString,
    hashOWString_Impl,
    equalOWString_Impl
> HashSet_OWString;

typedef hash_multimap
<
    OUString,
    Reference<XInterface >,
    hashOWString_Impl,
    equalOWString_Impl
> HashMultimap_OWString_Interface;

typedef hash_map
<
    OUString,
    Reference<XInterface >,
    hashOWString_Impl,
    equalOWString_Impl
> HashMap_OWString_Interface;

/*****************************************************************************
    class OServiceManager_Listener
*****************************************************************************/
class OServiceManager_Listener : public WeakImplHelper1< XEventListener >
{
private:
    WeakReference<XSet > xSMgr;

public:
    OServiceManager_Listener( const Reference<XSet > & rSMgr )
        : xSMgr( rSMgr )
        {}

    // XEventListener
    virtual void SAL_CALL disposing(const EventObject & rEvt ) throw(::com::sun::star::uno::RuntimeException);
};

void OServiceManager_Listener::disposing(const EventObject & rEvt )
    throw(::com::sun::star::uno::RuntimeException)
{
    Reference<XSet > x( xSMgr );
    if( x.is() )
    {
        try
        {
            x->remove( Any( &rEvt.Source, ::getCppuType( (const Reference<XInterface > *)0 ) ) );
        }
        catch( const IllegalArgumentException & )
        {
            OSL_ENSURE( sal_False, "IllegalArgumentException catched" );
        }
        catch( const NoSuchElementException & )
        {
            OSL_ENSURE( sal_False, "NoSuchElementException catched" );
        }
    }
}


/*****************************************************************************
    class OServiceManager
*****************************************************************************/
struct OServiceManagerMutex
{
    Mutex m_mutex;
};

extern "C" void SAL_CALL smgrUnloadingListener(void* id);

typedef WeakComponentImplHelper8<
    lang::XMultiServiceFactory, lang::XMultiComponentFactory, lang::XServiceInfo,
    lang::XInitialization, lang::XUnoTunnel,
    container::XSet, container::XContentEnumerationAccess,
    beans::XPropertySet > t_OServiceManager_impl;

class OServiceManager
    : public OServiceManagerMutex
    , public t_OServiceManager_impl
{
public:
    friend void SAL_CALL smgrUnloadingListener(void* id);

    OServiceManager( Reference< XComponentContext > const & xContext );
    virtual ~OServiceManager();

    // XUnoTunnel
    sal_Int64 SAL_CALL getSomething( Sequence< sal_Int8 > const & id )
        throw (RuntimeException);

    // XInitialization
    void SAL_CALL initialize( Sequence< Any > const & args )
        throw (Exception);

    // XServiceInfo
    virtual OUString SAL_CALL getImplementationName() throw(::com::sun::star::uno::RuntimeException);
    static OUString getImplementationName_Static() throw(::com::sun::star::uno::RuntimeException)
        { return smgr_getImplementationName(); }
    virtual sal_Bool SAL_CALL supportsService(const OUString& ServiceName) throw(::com::sun::star::uno::RuntimeException);
    virtual Sequence< OUString > SAL_CALL getSupportedServiceNames() throw(::com::sun::star::uno::RuntimeException);

    // XMultiComponentFactory
    virtual Reference< XInterface > SAL_CALL createInstanceWithContext(
        OUString const & rServiceSpecifier, Reference< XComponentContext > const & xContext )
        throw (Exception, RuntimeException);
    virtual Reference< XInterface > SAL_CALL createInstanceWithArgumentsAndContext(
        OUString const & rServiceSpecifier,
        Sequence< Any > const & rArguments,
        Reference< XComponentContext > const & xContext )
        throw (Exception, RuntimeException);
//      virtual Sequence< OUString > SAL_CALL getAvailableServiceNames()
//          throw (RuntimeException);

    // XMultiServiceFactory
    virtual Sequence< OUString > SAL_CALL getAvailableServiceNames() throw(::com::sun::star::uno::RuntimeException);
    virtual Reference<XInterface > SAL_CALL createInstance(const OUString &) throw(::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
    virtual Reference<XInterface > SAL_CALL createInstanceWithArguments(const OUString &, const Sequence<Any >& Arguments) throw(::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);

    // The same as the interface method, but only uique names
    Sequence< OUString > getAvailableServiceNames( HashSet_OWString & aNameSet );

    // XElementAccess
    virtual Type SAL_CALL getElementType() throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL hasElements() throw(::com::sun::star::uno::RuntimeException);

    // XEnumerationAccess
    virtual Reference<XEnumeration > SAL_CALL createEnumeration() throw(::com::sun::star::uno::RuntimeException);

    // XSet
    virtual sal_Bool SAL_CALL has( const Any & Element ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL insert( const Any & Element ) throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::container::ElementExistException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL remove( const Any & Element ) throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::container::NoSuchElementException, ::com::sun::star::uno::RuntimeException);

    // XContentEnumerationAccess
    //Sequence< OUString >			getAvailableServiceNames() throw( (Exception) );
    virtual Reference<XEnumeration > SAL_CALL createContentEnumeration(const OUString& aServiceName) throw(::com::sun::star::uno::RuntimeException);
    virtual Reference<XEnumeration > SAL_CALL createContentEnumeration(
        const OUString& aServiceName, Reference< XComponentContext > const & xContext )
        throw(::com::sun::star::uno::RuntimeException);

    // XComponent
    virtual void SAL_CALL dispose() throw(::com::sun::star::uno::RuntimeException);

    // XPropertySet
    Reference<XPropertySetInfo > SAL_CALL getPropertySetInfo()
        throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL setPropertyValue(const OUString& PropertyName, const Any& aValue)
        throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    Any SAL_CALL getPropertyValue(const OUString& PropertyName)
        throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    void SAL_CALL addPropertyChangeListener(const OUString& PropertyName, const Reference<XPropertyChangeListener >& aListener)
        throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    void SAL_CALL removePropertyChangeListener(const OUString& PropertyName, const Reference<XPropertyChangeListener >& aListener)
        throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    void SAL_CALL addVetoableChangeListener(const OUString& PropertyName, const Reference<XVetoableChangeListener >& aListener)
        throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    void SAL_CALL removeVetoableChangeListener(const OUString& PropertyName, const Reference<XVetoableChangeListener >& aListener)
        throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

protected:
    inline bool is_disposed() const SAL_THROW( (lang::DisposedException) );
    inline void check_undisposed() const SAL_THROW( (lang::DisposedException) );
    virtual void SAL_CALL disposing();

    sal_Bool haveFactoryWithThisImplementation(const OUString& aImplName);

    virtual Sequence< Reference< XInterface > > queryServiceFactories(
        const OUString& aServiceName, Reference< XComponentContext > const & xContext );

    Reference< XComponentContext >  m_xContext;

    Reference< beans::XPropertySetInfo > m_xPropertyInfo;

    sal_Int32 m_nUnloadingListenerId;

    // Does clean up when the unloading mechanism has been set off. It is called from
    // the listener function smgrUnloadingListener.
    void onUnloadingNotify();
    // factories which have been loaded and not inserted( by XSet::insert)
    // are remembered by this set. Those factories
    // are not released on a call to onUnloadingNotify
    HashSet_Ref m_SetLoadedFactories;
private:

    Reference<XEventListener >		getFactoryListener();


    HashMultimap_OWString_Interface	m_ServiceMap;
    HashSet_Ref						m_ImplementationMap;
    HashMap_OWString_Interface		m_ImplementationNameMap;
    Reference<XEventListener >		xFactoryListener;
    bool                            m_bInDisposing;
};


//______________________________________________________________________________
inline bool OServiceManager::is_disposed() const
    SAL_THROW( (lang::DisposedException) )
{
    // ought to be guarded by m_mutex:
    return (m_bInDisposing || rBHelper.bDisposed);
}

//______________________________________________________________________________
inline void OServiceManager::check_undisposed() const
    SAL_THROW( (lang::DisposedException) )
{
    if (is_disposed())
    {
        throw lang::DisposedException(
            OUSTR("service manager instance has already been disposed!"),
            (OWeakObject *)this );
    }
}

//##################################################################################################
//##################################################################################################
//##################################################################################################

class OServiceManagerWrapper : public OServiceManagerMutex, public t_OServiceManager_impl
{
    Reference< XComponentContext > m_xContext;
    OServiceManager * m_root;
    inline OServiceManager * getRoot() SAL_THROW( (RuntimeException) )
    {
        if (! m_root)
        {
            throw lang::DisposedException(
                OUSTR("service manager instance has already been disposed!"),
                Reference< XInterface >() );
        }
        return m_root;
    }

protected:
    virtual void SAL_CALL disposing();

public:
    OServiceManagerWrapper(
        Reference< XComponentContext > const & xContext )
        SAL_THROW( (RuntimeException) );
    virtual ~OServiceManagerWrapper() SAL_THROW( () );

    // XUnoTunnel
    sal_Int64 SAL_CALL getSomething( Sequence< sal_Int8 > const & id ) throw (RuntimeException)
        { return getRoot()->getSomething( id ); }

    // XInitialization
    void SAL_CALL initialize( Sequence< Any > const & args ) throw (Exception)
        { getRoot()->initialize( args ); }

    // XServiceInfo
    virtual OUString SAL_CALL getImplementationName() throw (RuntimeException)
        { return getRoot()->getImplementationName(); }
    virtual sal_Bool SAL_CALL supportsService(const OUString& ServiceName) throw (RuntimeException)
        { return getRoot()->supportsService( ServiceName ); }
    virtual Sequence< OUString > SAL_CALL getSupportedServiceNames() throw (RuntimeException)
        { return getRoot()->getSupportedServiceNames(); }

    // XMultiComponentFactory
    virtual Reference< XInterface > SAL_CALL createInstanceWithContext(
        OUString const & rServiceSpecifier, Reference< XComponentContext > const & xContext )
        throw (Exception, RuntimeException)
        { return getRoot()->createInstanceWithContext( rServiceSpecifier, xContext ); }
    virtual Reference< XInterface > SAL_CALL createInstanceWithArgumentsAndContext(
        OUString const & rServiceSpecifier,
        Sequence< Any > const & rArguments,
        Reference< XComponentContext > const & xContext )
        throw (Exception, RuntimeException)
        { return getRoot()->createInstanceWithArgumentsAndContext( rServiceSpecifier, rArguments, xContext ); }
//      virtual Sequence< OUString > SAL_CALL getAvailableServiceNames()
//          throw (RuntimeException);

    // XMultiServiceFactory
    virtual Sequence< OUString > SAL_CALL getAvailableServiceNames() throw (RuntimeException)
        { return getRoot()->getAvailableServiceNames(); }
    virtual Reference<XInterface > SAL_CALL createInstance(const OUString & name) throw (Exception)
        { return getRoot()->createInstanceWithContext( name, m_xContext ); }
    virtual Reference<XInterface > SAL_CALL createInstanceWithArguments(const OUString & name, const Sequence<Any >& Arguments) throw (Exception)
        { return getRoot()->createInstanceWithArgumentsAndContext( name, Arguments, m_xContext ); }

    // XElementAccess
    virtual Type SAL_CALL getElementType() throw (RuntimeException)
        { return getRoot()->getElementType(); }
    virtual sal_Bool SAL_CALL hasElements() throw (RuntimeException)
        { return getRoot()->hasElements(); }

    // XEnumerationAccess
    virtual Reference<XEnumeration > SAL_CALL createEnumeration() throw (RuntimeException)
        { return getRoot()->createEnumeration(); }

    // XSet
    virtual sal_Bool SAL_CALL has( const Any & Element ) throw (RuntimeException)
        { return getRoot()->has( Element ); }
    virtual void SAL_CALL insert( const Any & Element ) throw (lang::IllegalArgumentException, container::ElementExistException, RuntimeException)
        { getRoot()->insert( Element ); }
    virtual void SAL_CALL remove( const Any & Element ) throw (lang::IllegalArgumentException, container::NoSuchElementException, RuntimeException)
        { getRoot()->remove( Element ); }

    // XContentEnumerationAccess
    //Sequence< OUString >			getAvailableServiceNames() throw( (Exception) );
    virtual Reference<XEnumeration > SAL_CALL createContentEnumeration(const OUString& aServiceName) throw (RuntimeException)
        { return getRoot()->createContentEnumeration( aServiceName, m_xContext ); }

    // XPropertySet
    Reference<XPropertySetInfo > SAL_CALL getPropertySetInfo() throw (RuntimeException)
        { return getRoot()->getPropertySetInfo(); }

    void SAL_CALL setPropertyValue(const OUString& PropertyName, const Any& aValue)
        throw (beans::UnknownPropertyException, beans::PropertyVetoException, lang::IllegalArgumentException, lang::WrappedTargetException, RuntimeException);
    Any SAL_CALL getPropertyValue(const OUString& PropertyName)
        throw (beans::UnknownPropertyException, lang::WrappedTargetException, RuntimeException);

    void SAL_CALL addPropertyChangeListener(const OUString& PropertyName, const Reference<XPropertyChangeListener >& aListener)
        throw (beans::UnknownPropertyException, lang::WrappedTargetException, RuntimeException)
        { getRoot()->addPropertyChangeListener( PropertyName, aListener ); }
    void SAL_CALL removePropertyChangeListener(const OUString& PropertyName, const Reference<XPropertyChangeListener >& aListener)
        throw (beans::UnknownPropertyException, lang::WrappedTargetException, RuntimeException)
        { getRoot()->removePropertyChangeListener( PropertyName, aListener ); }
    void SAL_CALL addVetoableChangeListener(const OUString& PropertyName, const Reference<XVetoableChangeListener >& aListener)
        throw (beans::UnknownPropertyException, lang::WrappedTargetException, RuntimeException)
        { getRoot()->addVetoableChangeListener( PropertyName, aListener ); }
    void SAL_CALL removeVetoableChangeListener(const OUString& PropertyName, const Reference<XVetoableChangeListener >& aListener)
        throw (beans::UnknownPropertyException, lang::WrappedTargetException, RuntimeException)
        { getRoot()->removeVetoableChangeListener( PropertyName, aListener ); }
};
//__________________________________________________________________________________________________
void SAL_CALL OServiceManagerWrapper::setPropertyValue(
    const OUString& PropertyName, const Any& aValue )
    throw (beans::UnknownPropertyException, beans::PropertyVetoException,
           lang::IllegalArgumentException, lang::WrappedTargetException, RuntimeException)
{
    if (PropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("DefaultContext") ))
    {
        Reference< XComponentContext > xContext;
        if (aValue >>= xContext)
        {
            MutexGuard aGuard( m_mutex );
            m_xContext = xContext;
        }
        else
        {
            throw IllegalArgumentException(
                OUString( RTL_CONSTASCII_USTRINGPARAM("no XComponentContext given!") ),
                (OWeakObject *)this, 1 );
        }
    }
    else
    {
        getRoot()->setPropertyValue( PropertyName, aValue );
    }
}
//__________________________________________________________________________________________________
Any SAL_CALL OServiceManagerWrapper::getPropertyValue(
    const OUString& PropertyName )
    throw (beans::UnknownPropertyException, lang::WrappedTargetException, RuntimeException)
{
    if (PropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("DefaultContext") ))
    {
        MutexGuard aGuard( m_mutex );
        if( m_xContext.is() )
            return makeAny( m_xContext );
        else
            return Any();
    }
    else
    {
        return getRoot()->getPropertyValue( PropertyName );
    }
}
//__________________________________________________________________________________________________
void OServiceManagerWrapper::disposing()
{
    m_xContext.clear();

    if (m_root)
    {
// no m_root->dispose(), because every context disposes its service manager...
        m_root->release();
        m_root = 0;
    }
}
//__________________________________________________________________________________________________
OServiceManagerWrapper::~OServiceManagerWrapper() SAL_THROW( () )
{
    if (m_root)
    {
        m_root->release();
        m_root = 0;
    }

    g_moduleCount.modCnt.release( &g_moduleCount.modCnt );
}
//__________________________________________________________________________________________________
OServiceManagerWrapper::OServiceManagerWrapper(
    Reference< XComponentContext > const & xContext )
    SAL_THROW( (RuntimeException) )
    : t_OServiceManager_impl( m_mutex )
    , m_xContext( xContext )
    , m_root( 0 )
{
    g_moduleCount.modCnt.acquire( &g_moduleCount.modCnt );

    Reference< XUnoTunnel > xTunnel( m_xContext->getServiceManager(), UNO_QUERY );
    OSL_ASSERT( xTunnel.is() );
    if (xTunnel.is())
    {
        m_root = reinterpret_cast< OServiceManager * >(
            xTunnel->getSomething( smgr_getImplementationId() ) );
        OSL_ASSERT( m_root );
        if (m_root)
        {
            m_root->acquire();
        }
    }

    if (! m_root)
    {
        throw RuntimeException(
            OUString( RTL_CONSTASCII_USTRINGPARAM("can only wrap OServiceManager instances!") ),
            Reference< XInterface >() );
    }
}

static Reference<XInterface > SAL_CALL OServiceManagerWrapper_CreateInstance(
    const Reference< XComponentContext > & xContext )
    throw (Exception)
{
    return (OWeakObject *)new OServiceManagerWrapper( xContext );
}

//##################################################################################################
//##################################################################################################
//##################################################################################################

// XUnoTunnel
sal_Int64 OServiceManager::getSomething( Sequence< sal_Int8 > const & id )
    throw (RuntimeException)
{
    check_undisposed();
    if (id == smgr_getImplementationId())
        return reinterpret_cast< sal_Int64 >(this);
    else
        return 0;
}

/**
 * Create a ServiceManager
 */
OServiceManager::OServiceManager( Reference< XComponentContext > const & xContext )
    : t_OServiceManager_impl( m_mutex )
    , m_xContext( xContext )
    , m_bInDisposing( false )
{
    g_moduleCount.modCnt.acquire( &g_moduleCount.modCnt );
    m_nUnloadingListenerId= rtl_addUnloadingListener( smgrUnloadingListener, this);
}

/**
 * Destroy the ServiceManager
 */
OServiceManager::~OServiceManager()
{
    if( m_nUnloadingListenerId != 0)
        rtl_removeUnloadingListener( m_nUnloadingListenerId );

    g_moduleCount.modCnt.release( &g_moduleCount.modCnt );
}

// Removes entries in m_ServiceMap, m_ImplementationNameMap and m_ImplementationNameMap
// if those entries have not been inserted through XSet::insert. Therefore the entries
// are compared with the entries in m_SetLoadedFactories.
void OServiceManager::onUnloadingNotify()
{
    MutexGuard aGuard( m_mutex);

    typedef HashSet_Ref::const_iterator CIT_S;
    typedef HashMultimap_OWString_Interface::iterator IT_MM;

    CIT_S it_SetEnd= m_SetLoadedFactories.end();
    IT_MM it_end1= m_ServiceMap.end();
    list<IT_MM> listDeleteServiceMap;
    typedef list<IT_MM>::const_iterator CIT_DMM;
    // find occurences in m_ServiceMap
    for(IT_MM it_i1= m_ServiceMap.begin(); it_i1 != it_end1; it_i1++)
    {
        if( m_SetLoadedFactories.find( it_i1->second) != it_SetEnd)
        {
            Reference<XUnloadingPreference> xunl( it_i1->second, UNO_QUERY);
            if( xunl.is())
            {
                if( xunl->releaseOnNotification())
                    listDeleteServiceMap.push_front( it_i1);
            }
            else
                listDeleteServiceMap.push_front( it_i1);
        }
    }
    // delete elements from m_ServiceMap
    CIT_DMM it_end2= listDeleteServiceMap.end();
    for( CIT_DMM it_i2= listDeleteServiceMap.begin(); it_i2 != it_end2; it_i2++)
        m_ServiceMap.erase( *it_i2);

    // find elements in m_ImplementationNameMap
    typedef HashMap_OWString_Interface::iterator IT_M;
    IT_M it_end3= m_ImplementationNameMap.end();
    list<IT_M> listDeleteImplementationNameMap;
    typedef list<IT_M>::const_iterator CIT_DM;
    for( IT_M it_i3= m_ImplementationNameMap.begin();  it_i3 != it_end3; it_i3++)
    {
        if( m_SetLoadedFactories.find( it_i3->second) != it_SetEnd)
        {
            Reference<XUnloadingPreference> xunl( it_i3->second, UNO_QUERY);
            if( xunl.is())
            {
                if( xunl->releaseOnNotification())
                    listDeleteImplementationNameMap.push_front( it_i3);
            }
            else
                listDeleteImplementationNameMap.push_front( it_i3);
        }
    }
    // delete elements from m_ImplementationNameMap
    CIT_DM it_end4= listDeleteImplementationNameMap.end();
    for( CIT_DM it_i4= listDeleteImplementationNameMap.begin(); it_i4 != it_end4; it_i4++)
        m_ImplementationNameMap.erase( *it_i4);

    // find elements in m_ImplementationMap
    typedef HashSet_Ref::iterator IT_S;
    IT_S it_end5= m_ImplementationMap.end();
    list<IT_S> listDeleteImplementationMap;
    typedef list<IT_S>::const_iterator CIT_DS;
    for( IT_S it_i5= m_ImplementationMap.begin(); it_i5 != it_end5; it_i5++)
    {
        if( m_SetLoadedFactories.find( *it_i5) != it_SetEnd)
        {
            Reference<XUnloadingPreference> xunl( *it_i5, UNO_QUERY);
            if( xunl.is())
            {
                if( xunl->releaseOnNotification())
                    listDeleteImplementationMap.push_front( it_i5);
            }
            else
                listDeleteImplementationMap.push_front( it_i5);
        }
    }
    // delete elements from m_ImplementationMap
    CIT_DS it_end6= listDeleteImplementationMap.end();
    for( CIT_DS it_i6= listDeleteImplementationMap.begin(); it_i6 != it_end6; it_i6++)
        m_ImplementationMap.erase( *it_i6);

    // remove Event listener before the factories are released.
    IT_S it_end7= m_SetLoadedFactories.end();

    Reference<XEventListener> xlistener= getFactoryListener();
    for( IT_S it_i7= m_SetLoadedFactories.begin(); it_i7 != it_end7; it_i7++)
    {
        Reference<XComponent> xcomp( *it_i7, UNO_QUERY);
        if( xcomp.is())
            xcomp->removeEventListener( xlistener);
    }
    // release the factories in m_SetLoadedFactories
    m_SetLoadedFactories.clear();
}

// XComponent
void OServiceManager::dispose()
    throw(::com::sun::star::uno::RuntimeException)
{
    if (rBHelper.bDisposed || rBHelper.bInDispose)
        return;
    t_OServiceManager_impl::dispose();
}

void OServiceManager::disposing()
{
    // dispose all factories
    HashSet_Ref aImpls;
    {
        MutexGuard aGuard( m_mutex );
        m_bInDisposing = true;
        aImpls = m_ImplementationMap;
    }
    HashSet_Ref::iterator aIt = aImpls.begin();
    while( aIt != aImpls.end() )
    {
        try
        {
            Reference<XComponent > xComp( Reference<XComponent >::query( *aIt++ ) );
            if( xComp.is() )
                xComp->dispose();
        }
        catch (RuntimeException & exc)
        {
#if OSL_DEBUG_LEVEL > 1
            OString str( OUStringToOString( exc.Message, RTL_TEXTENCODING_ASCII_US ) );
            OSL_TRACE( "### RuntimeException occured upon disposing factory: %s", str.getStr() );
#endif
        }
    }

    // dispose
    HashSet_Ref aImplMap;
    {
        MutexGuard aGuard( m_mutex );
        // erase all members
        m_ServiceMap = HashMultimap_OWString_Interface();
        aImplMap = m_ImplementationMap;
        m_ImplementationMap = HashSet_Ref();
        m_ImplementationNameMap = HashMap_OWString_Interface();
        m_SetLoadedFactories= HashSet_Ref();
    }

    m_xContext.clear();

    // not only the Event should hold the object
    OSL_ASSERT( m_refCount != 1 );

    // Revoke this service manager as unloading listener
    rtl_removeUnloadingListener( m_nUnloadingListenerId);
    m_nUnloadingListenerId=0;
}

// XPropertySet
Reference<XPropertySetInfo > OServiceManager::getPropertySetInfo()
    throw(::com::sun::star::uno::RuntimeException)
{
    check_undisposed();
    if (! m_xPropertyInfo.is())
    {
        Sequence< beans::Property > seq( 1 );
        seq[ 0 ] = beans::Property(
            OUSTR("DefaultContext"), -1, ::getCppuType( &m_xContext ), 0 );
        Reference< beans::XPropertySetInfo > xInfo( new PropertySetInfo_Impl( seq ) );

        MutexGuard aGuard( m_mutex );
        if (! m_xPropertyInfo.is())
        {
            m_xPropertyInfo = xInfo;
        }
    }
    return m_xPropertyInfo;
}

void OServiceManager::setPropertyValue(
    const OUString& PropertyName, const Any& aValue )
    throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
{
    check_undisposed();
    if (PropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("DefaultContext") ))
    {
        Reference< XComponentContext > xContext;
        if (aValue >>= xContext)
        {
            MutexGuard aGuard( m_mutex );
            m_xContext = xContext;
        }
        else
        {
            throw IllegalArgumentException(
                OUString( RTL_CONSTASCII_USTRINGPARAM("no XComponentContext given!") ),
                (OWeakObject *)this, 1 );
        }
    }
    else
    {
        throw UnknownPropertyException(
            OUString( RTL_CONSTASCII_USTRINGPARAM("unknown property ") ) + PropertyName,
            (OWeakObject *)this );
    }
}

Any OServiceManager::getPropertyValue(const OUString& PropertyName)
    throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
{
    check_undisposed();
    if (PropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("DefaultContext") ))
    {
        MutexGuard aGuard( m_mutex );
        if( m_xContext.is() )
            return makeAny( m_xContext );
        else
            return Any();
    }
    else
    {
        UnknownPropertyException except;
        except.Message = OUString( RTL_CONSTASCII_USTRINGPARAM( "ServiceManager : unknown property " ) );
        except.Message += PropertyName;
        throw except;
    }
}

void OServiceManager::addPropertyChangeListener(
    const OUString& PropertyName,
    const Reference<XPropertyChangeListener >& aListener)
    throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
{
    check_undisposed();
    throw UnknownPropertyException();
}

void OServiceManager::removePropertyChangeListener(
    const OUString& PropertyName,
    const Reference<XPropertyChangeListener >& aListener)
    throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
{
    check_undisposed();
    throw UnknownPropertyException();
}

void OServiceManager::addVetoableChangeListener(
    const OUString& PropertyName,
    const Reference<XVetoableChangeListener >& aListener)
    throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
{
    check_undisposed();
    throw UnknownPropertyException();
}

void OServiceManager::removeVetoableChangeListener(
    const OUString& PropertyName,
    const Reference<XVetoableChangeListener >& aListener)
    throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
{
    check_undisposed();
    throw UnknownPropertyException();
}

// OServiceManager
Reference<XEventListener > OServiceManager::getFactoryListener()
{
    check_undisposed();
    MutexGuard aGuard( m_mutex );
    if( !xFactoryListener.is() )
        xFactoryListener = new OServiceManager_Listener( this );
    return xFactoryListener;
}

// XMultiServiceFactory, XContentEnumeration
Sequence< OUString > OServiceManager::getAvailableServiceNames( HashSet_OWString & aNameSet )
{
    check_undisposed();
    MutexGuard aGuard( m_mutex );
    HashMultimap_OWString_Interface::iterator aSIt = m_ServiceMap.begin();
    while( aSIt != m_ServiceMap.end() )
        aNameSet.insert( (*aSIt++).first );

    /* do not return the implementation names
    HashMap_OWString_Interface		m_ImplementationNameMap;
    HashMap_OWString_Interface::iterator aIt = m_ImplementationNameMap.begin();
    while( aIt != m_ImplementationNameMap.end() )
        aNameSet.insert( (*aIt++).first );
    */

    Sequence< OUString > aNames( aNameSet.size() );
    OUString * pArray = aNames.getArray();
    sal_Int32 i = 0;
    HashSet_OWString::iterator next = aNameSet.begin();
    while( next != aNameSet.end() )
        pArray[i++] = (*next++);

    return aNames;
}

// XMultiComponentFactory
Reference< XInterface > OServiceManager::createInstanceWithContext(
    OUString const & rServiceSpecifier,
    Reference< XComponentContext > const & xContext )
    throw (Exception, RuntimeException)
{
    check_undisposed();
#if OSL_DEBUG_LEVEL > 0
    Reference< beans::XPropertySet > xProps( xContext->getServiceManager(), UNO_QUERY );
    OSL_ASSERT( xProps.is() );
    if (xProps.is())
    {
        Reference< XComponentContext > xDefContext;
        xProps->getPropertyValue(
            OUString( RTL_CONSTASCII_USTRINGPARAM("DefaultContext") ) ) >>= xDefContext;
        OSL_ENSURE(
            xContext == xDefContext,
            "### default context of service manager singleton differs from context holding it!" );
    }
#endif

    Sequence< Reference< XInterface > > factories(
        queryServiceFactories( rServiceSpecifier, xContext ) );
    Reference< XInterface > const * p = factories.getConstArray();
    for ( sal_Int32 nPos = 0; nPos < factories.getLength(); ++nPos )
    {
        try
        {
            Reference< XInterface > const & xFactory = p[ nPos ];
            if (xFactory.is())
            {
                Reference< XSingleComponentFactory > xFac( xFactory, UNO_QUERY );
                if (xFac.is())
                {
                    return xFac->createInstanceWithContext( xContext );
                }
                else
                {
                    Reference< XSingleServiceFactory > xFac( xFactory, UNO_QUERY );
                    if (xFac.is())
                    {
#if OSL_DEBUG_LEVEL > 1
                        OString aStr( OUStringToOString( rServiceSpecifier, RTL_TEXTENCODING_ASCII_US ) );
                        OSL_TRACE( "### ignoring given context raising service %s !!!\n", aStr.getStr() );
#endif
                        return xFac->createInstance();
                    }
                }
            }
        }
        catch (lang::DisposedException & exc)
        {
#if OSL_DEBUG_LEVEL > 1
            OString str( OUStringToOString( exc.Message, RTL_TEXTENCODING_ASCII_US ) );
            OSL_TRACE( "### DisposedException occured: %s", str.getStr() );
#endif
        }
    }

    return Reference< XInterface >();
}
// XMultiComponentFactory
Reference< XInterface > OServiceManager::createInstanceWithArgumentsAndContext(
    OUString const & rServiceSpecifier,
    Sequence< Any > const & rArguments,
    Reference< XComponentContext > const & xContext )
    throw (Exception, RuntimeException)
{
    check_undisposed();
#if OSL_DEBUG_LEVEL > 0
    Reference< beans::XPropertySet > xProps( xContext->getServiceManager(), UNO_QUERY );
    OSL_ASSERT( xProps.is() );
    if (xProps.is())
    {
        Reference< XComponentContext > xDefContext;
        xProps->getPropertyValue(
            OUString( RTL_CONSTASCII_USTRINGPARAM("DefaultContext") ) ) >>= xDefContext;
        OSL_ENSURE(
            xContext == xDefContext,
            "### default context of service manager singleton differs from context holding it!" );
    }
#endif

    Sequence< Reference< XInterface > > factories(
        queryServiceFactories( rServiceSpecifier, xContext ) );
    Reference< XInterface > const * p = factories.getConstArray();
    for ( sal_Int32 nPos = 0; nPos < factories.getLength(); ++nPos )
    {
        try
        {
            Reference< XInterface > const & xFactory = p[ nPos ];
            if (xFactory.is())
            {
                Reference< XSingleComponentFactory > xFac( xFactory, UNO_QUERY );
                if (xFac.is())
                {
                    return xFac->createInstanceWithArgumentsAndContext( rArguments, xContext );
                }
                else
                {
                    Reference< XSingleServiceFactory > xFac( xFactory, UNO_QUERY );
                    if (xFac.is())
                    {
#if OSL_DEBUG_LEVEL > 1
                        OString aStr( OUStringToOString( rServiceSpecifier, RTL_TEXTENCODING_ASCII_US ) );
                        OSL_TRACE( "### ignoring given context raising service %s !!!\n", aStr.getStr() );
#endif
                        return xFac->createInstanceWithArguments( rArguments );
                    }
                }
            }
        }
        catch (lang::DisposedException & exc)
        {
#if OSL_DEBUG_LEVEL > 1
            OString str( OUStringToOString( exc.Message, RTL_TEXTENCODING_ASCII_US ) );
            OSL_TRACE( "### DisposedException occured: %s", str.getStr() );
#endif
        }
    }

    return Reference< XInterface >();
}

// XMultiServiceFactory, XMultiComponentFactory, XContentEnumeration
Sequence< OUString > OServiceManager::getAvailableServiceNames()
    throw(::com::sun::star::uno::RuntimeException)
{
    check_undisposed();
    // all names
    HashSet_OWString aNameSet;
    return getAvailableServiceNames( aNameSet );
}

// XMultibleServiceFactory
Reference<XInterface > OServiceManager::createInstance(
    const OUString& rServiceSpecifier )
    throw(::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException)
{
    return createInstanceWithContext(
        rServiceSpecifier, m_xContext );
}

// XMultibleServiceFactory
Reference<XInterface > OServiceManager::createInstanceWithArguments(
    const OUString& rServiceSpecifier,
    const Sequence<Any >& rArguments )
    throw(::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException)
{
    return createInstanceWithArgumentsAndContext(
        rServiceSpecifier, rArguments, m_xContext );
}

// XInitialization
void OServiceManager::initialize( Sequence< Any > const & )
    throw (Exception)
{
    check_undisposed();
    OSL_ENSURE( 0, "not impl!" );
}

// XServiceInfo
OUString OServiceManager::getImplementationName()
    throw(::com::sun::star::uno::RuntimeException)
{
    check_undisposed();
    return getImplementationName_Static();
}

// XServiceInfo
sal_Bool OServiceManager::supportsService(const OUString& ServiceName)
    throw(::com::sun::star::uno::RuntimeException)
{
    check_undisposed();
    Sequence< OUString > aSNL = getSupportedServiceNames();
    const OUString * pArray = aSNL.getConstArray();
    for( sal_Int32 i = 0; i < aSNL.getLength(); i++ )
        if( pArray[i] == ServiceName )
            return sal_True;
    return sal_False;
}

// XServiceInfo
Sequence< OUString > OServiceManager::getSupportedServiceNames()
    throw(::com::sun::star::uno::RuntimeException)
{
    check_undisposed();
    return smgr_getSupportedServiceNames();
}


Sequence< Reference< XInterface > > OServiceManager::queryServiceFactories(
    const OUString& aServiceName, Reference< XComponentContext > const & xContext )
{
    Sequence< Reference< XInterface > > ret;

    MutexGuard aGuard( m_mutex );
    ::std::pair<
          HashMultimap_OWString_Interface::iterator,
          HashMultimap_OWString_Interface::iterator> p(
              m_ServiceMap.equal_range( aServiceName ) );

    if (p.first == p.second) // no factories
    {
        // no service found, look for an implementation
        HashMap_OWString_Interface::iterator aIt = m_ImplementationNameMap.find( aServiceName );
        if( aIt != m_ImplementationNameMap.end() )
        {
            Reference< XInterface > const & x = aIt->second;
            // an implementation found
            ret = Sequence< Reference< XInterface > >( &x, 1 );
        }
    }
    else
    {
        ::std::vector< Reference< XInterface > > vec;
        vec.reserve( 4 );
        while (p.first != p.second)
        {
            vec.push_back( p.first->second );
            ++p.first;
        }
        ret = Sequence< Reference< XInterface > >( &vec[ 0 ], vec.size() );
    }

    return ret;
}

// XContentEnumerationAccess
Reference<XEnumeration > OServiceManager::createContentEnumeration(
    const OUString& aServiceName, Reference< XComponentContext > const & xContext )
    throw(::com::sun::star::uno::RuntimeException)
{
    check_undisposed();
    Sequence< Reference< XInterface > > factories(
        OServiceManager::queryServiceFactories( aServiceName, xContext ) );
    if (factories.getLength())
        return new ServiceEnumeration_Impl( factories );
    else
        return Reference< XEnumeration >();
}
Reference<XEnumeration > OServiceManager::createContentEnumeration(
    const OUString& aServiceName )
    throw(::com::sun::star::uno::RuntimeException)
{
    return createContentEnumeration( aServiceName, m_xContext );
}

// XEnumeration
Reference<XEnumeration > OServiceManager::createEnumeration() throw(::com::sun::star::uno::RuntimeException)
{
    check_undisposed();
    MutexGuard aGuard( m_mutex );
    return new ImplementationEnumeration_Impl( m_ImplementationMap );
}

// XElementAccess
Type OServiceManager::getElementType()
    throw(::com::sun::star::uno::RuntimeException)
{
    check_undisposed();
    return ::getCppuType( (const Reference< XInterface > *)0 );
}

// XElementAccess
sal_Bool OServiceManager::hasElements()
    throw(::com::sun::star::uno::RuntimeException)
{
    check_undisposed();
    MutexGuard aGuard( m_mutex );
    return !m_ImplementationMap.empty();
}

// XSet
sal_Bool OServiceManager::has( const Any & Element )
    throw(::com::sun::star::uno::RuntimeException)
{
    check_undisposed();
    if( Element.getValueTypeClass() == TypeClass_INTERFACE )
    {
        Reference<XInterface > xEle( Element, UNO_QUERY_THROW );
        MutexGuard aGuard( m_mutex );
        return m_ImplementationMap.find( xEle ) !=
            m_ImplementationMap.end();
    }
    else if (Element.getValueTypeClass() == TypeClass_STRING)
    {
        OUString const & implName =
            *reinterpret_cast< OUString const * >(Element.getValue());
        MutexGuard aGuard( m_mutex );
        return m_ImplementationNameMap.find( implName ) !=
            m_ImplementationNameMap.end();
    }
    return sal_False;
}

// XSet
void OServiceManager::insert( const Any & Element )
    throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::container::ElementExistException, ::com::sun::star::uno::RuntimeException)
{
    check_undisposed();
    if( Element.getValueTypeClass() != TypeClass_INTERFACE )
    {
        throw IllegalArgumentException(
            OUString( RTL_CONSTASCII_USTRINGPARAM("no interface given!") ),
            Reference< XInterface >(), 0 );
    }
    Reference<XInterface > xEle( Element, UNO_QUERY_THROW );

    {
    MutexGuard aGuard( m_mutex );
    HashSet_Ref::iterator aIt = m_ImplementationMap.find( xEle );
    if( aIt != m_ImplementationMap.end() )
    {
        throw ElementExistException(
            OUString( RTL_CONSTASCII_USTRINGPARAM("element already exists!") ),
            Reference< XInterface >() );
    }

    // put into the implementation hashmap
    m_ImplementationMap.insert( xEle );

    // put into the implementation name hashmap
    Reference<XServiceInfo > xInfo( Reference<XServiceInfo >::query( xEle ) );
    if( xInfo.is() )
    {
        OUString aImplName = xInfo->getImplementationName();
        if( aImplName.getLength() )
            m_ImplementationNameMap[ aImplName ] = xEle;
        
        //put into the service map
        Sequence< OUString > aServiceNames = xInfo->getSupportedServiceNames();
        const OUString * pArray = aServiceNames.getConstArray();
        for( sal_Int32 i = 0; i < aServiceNames.getLength(); i++ )
        {
            m_ServiceMap.insert( HashMultimap_OWString_Interface::value_type(
                pArray[i], *(Reference<XInterface > *)Element.getValue() ) );
        }
    }
    }
    // add the disposing listener to the factory
    Reference<XComponent > xComp( Reference<XComponent >::query( xEle ) );
    if( xComp.is() )
        xComp->addEventListener( getFactoryListener() );
}

// helper function
sal_Bool OServiceManager::haveFactoryWithThisImplementation(const OUString& aImplName)
{
    return ( m_ImplementationNameMap.find(aImplName) != m_ImplementationNameMap.end());
}

// XSet
void OServiceManager::remove( const Any & Element )
     throw(::com::sun::star::lang::IllegalArgumentException,
           ::com::sun::star::container::NoSuchElementException,
           ::com::sun::star::uno::RuntimeException)
{
    if (is_disposed())
        return;
    
    Reference<XInterface > xEle;
    if (Element.getValueTypeClass() == TypeClass_INTERFACE)
    {
        xEle.set( Element, UNO_QUERY_THROW );
    }
    else if (Element.getValueTypeClass() == TypeClass_STRING)
    {
        OUString const & implName =
            *reinterpret_cast< OUString const * >(Element.getValue());
        MutexGuard aGuard( m_mutex );
        HashMap_OWString_Interface::const_iterator const iFind(
            m_ImplementationNameMap.find( implName ) );
        if (iFind == m_ImplementationNameMap.end())
        {
            throw NoSuchElementException(
                OUString( RTL_CONSTASCII_USTRINGPARAM("element is not in: ") )
                + implName, static_cast< OWeakObject * >(this) );
        }
        xEle = iFind->second;
    }
    else
    {
        throw IllegalArgumentException(
            OUString( RTL_CONSTASCII_USTRINGPARAM(
                          "neither interface nor string given!") ),
            Reference< XInterface >(), 0 );
    }

    // remove the disposing listener from the factory
    Reference<XComponent > xComp( Reference<XComponent >::query( xEle ) );
    if( xComp.is() )
        xComp->removeEventListener( getFactoryListener() );

    MutexGuard aGuard( m_mutex );
    HashSet_Ref::iterator aIt = m_ImplementationMap.find( xEle );
    if( aIt == m_ImplementationMap.end() )
    {
        throw NoSuchElementException(
            OUString( RTL_CONSTASCII_USTRINGPARAM("element is not in!") ),
            static_cast< OWeakObject * >(this) );
    }

    // remove from the implementation map
    m_ImplementationMap.erase( aIt );

    m_SetLoadedFactories.erase( *aIt);
    // remove from the implementation name hashmap
    Reference<XServiceInfo > xInfo( Reference<XServiceInfo >::query( xEle ) );
    if( xInfo.is() )
    {
        OUString aImplName = xInfo->getImplementationName();
        if( aImplName.getLength() )
            m_ImplementationNameMap.erase( aImplName );
    }

    //remove from the service map
    Reference<XServiceInfo > xSF( Reference<XServiceInfo >::query( xEle ) );
    if( xSF.is() )
    {
        Sequence< OUString > aServiceNames = xSF->getSupportedServiceNames();
        const OUString * pArray = aServiceNames.getConstArray();
        for( sal_Int32 i = 0; i < aServiceNames.getLength(); i++ )
        {
            pair<HashMultimap_OWString_Interface::iterator, HashMultimap_OWString_Interface::iterator> p =
                m_ServiceMap.equal_range( pArray[i] );

            while( p.first != p.second )
            {
                if( xEle == (*p.first).second )
                {
                    m_ServiceMap.erase( p.first );
                    break;
                }
                ++p.first;
            }
        }
    }
}

/*****************************************************************************
    class ORegistryServiceManager
*****************************************************************************/
class ORegistryServiceManager : public OServiceManager
{
public:
    ORegistryServiceManager( Reference< XComponentContext > const & xContext );
    virtual ~ORegistryServiceManager();

    // XInitialization
    void SAL_CALL initialize(const Sequence< Any >& Arguments)
        throw(::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);

    // XServiceInfo
    OUString SAL_CALL getImplementationName() throw(::com::sun::star::uno::RuntimeException)
        { return regsmgr_getImplementationName(); }

    Sequence< OUString > SAL_CALL getSupportedServiceNames() throw(::com::sun::star::uno::RuntimeException);

    // XMultiServiceFactory
    Sequence< OUString > SAL_CALL getAvailableServiceNames() throw(::com::sun::star::uno::RuntimeException);

    // XContentEnumerationAccess
    //Sequence< OUString >			getAvailableServiceNames() throw( (Exception) );
    Reference<XEnumeration > SAL_CALL createContentEnumeration(const OUString& aServiceName) throw(::com::sun::star::uno::RuntimeException);
    virtual Reference<XEnumeration > SAL_CALL createContentEnumeration(
        const OUString& aServiceName, Reference< XComponentContext > const & xContext )
        throw(::com::sun::star::uno::RuntimeException);

    // XComponent
    void SAL_CALL dispose() throw(::com::sun::star::uno::RuntimeException);

    // OServiceManager
    Reference<XPropertySetInfo > SAL_CALL getPropertySetInfo()
        throw(::com::sun::star::uno::RuntimeException);
    Any SAL_CALL getPropertyValue(const OUString& PropertyName)
        throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

protected:
    //OServiceManager
    Sequence< Reference< XInterface > > queryServiceFactories(
        const OUString& aServiceName, Reference< XComponentContext > const & xContext );
private:
    Reference<XRegistryKey >		getRootKey();
    Reference<XInterface > loadWithImplementationName(
        const OUString & rImplName, Reference< XComponentContext > const & xContext );
    Sequence<OUString>			getFromServiceName(const OUString& serviceName);
    Reference<XInterface > loadWithServiceName(
        const OUString & rImplName, Reference< XComponentContext > const & xContext );
    void						fillAllNamesFromRegistry( HashSet_OWString & );

    sal_Bool					m_searchedRegistry;
    Reference<XSimpleRegistry > m_xRegistry;	// readonly property Registry
    Reference<XRegistryKey >	m_xRootKey;

#if OSL_DEBUG_LEVEL > 0
    bool m_init;
#endif
};

/**
 * Create a ServiceManager
 */
ORegistryServiceManager::ORegistryServiceManager( Reference< XComponentContext > const & xContext )
    : OServiceManager( xContext )
    , m_searchedRegistry(sal_False)
#if OSL_DEBUG_LEVEL > 0
    , m_init( false )
#endif
{
}

/**
 * Destroy the ServiceManager
 */
ORegistryServiceManager::~ORegistryServiceManager()
{
}

// XComponent
void ORegistryServiceManager::dispose()
    throw(::com::sun::star::uno::RuntimeException)
{
    if (rBHelper.bDisposed || rBHelper.bInDispose)
        return;
    OServiceManager::dispose();
    // dispose
    MutexGuard aGuard( m_mutex );
    // erase all members
    m_xRegistry = Reference<XSimpleRegistry >();
    m_xRootKey = Reference<XRegistryKey >();
}

/**
 * Return the root key of the registry. The Default registry service is ordered
 * if no registry is set.
 */
//Reference<XServiceProvider > create_DefaultRegistry_ServiceProvider();

Reference<XRegistryKey > ORegistryServiceManager::getRootKey()
{
    if( !m_xRootKey.is() )
    {
        MutexGuard aGuard( m_mutex );
        //	DefaultRegistry suchen !!!!
        if( !m_xRegistry.is() && !m_searchedRegistry )
        {
            // merken, es wird nur einmal gesucht
            m_searchedRegistry = sal_True;

            m_xRegistry.set(
                createInstanceWithContext(
                    OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.registry.DefaultRegistry") ),
                    m_xContext ),
                UNO_QUERY );
        }
        if( m_xRegistry.is() && !m_xRootKey.is() )
            m_xRootKey = m_xRegistry->getRootKey();
    }

    return m_xRootKey;
}

/**
 * Create a service provider from the registry with an implementation name
 */
Reference<XInterface > ORegistryServiceManager::loadWithImplementationName(
    const OUString& name, Reference< XComponentContext > const & xContext )
{
    Reference<XInterface > ret;

    Reference<XRegistryKey > xRootKey = getRootKey();
    if( !xRootKey.is() )
        return ret;

    try
    {
        OUString implementationName = OUString( RTL_CONSTASCII_USTRINGPARAM("/IMPLEMENTATIONS/") ) + name;
        Reference<XRegistryKey > xImpKey = m_xRootKey->openKey(implementationName);

        if( xImpKey.is() )
        {
            Reference< lang::XMultiServiceFactory > xMgr;
            if (xContext.is())
                xMgr.set( xContext->getServiceManager(), UNO_QUERY_THROW );
            else
                xMgr.set( this );
            ret = createSingleRegistryFactory( xMgr, name, xImpKey );
            insert( makeAny( ret ) );
            // Remember this factory as loaded in contrast to inserted ( XSet::insert)
            // factories. Those loaded factories in this set are candidates for being
            // released on an unloading notification.
            m_SetLoadedFactories.insert( ret);
        }
    }
    catch (InvalidRegistryException &)
    {
    }

    return ret;
}

/**
 * Return all implementation out of the registry.
 */
Sequence<OUString> ORegistryServiceManager::getFromServiceName(
    const OUString& serviceName )
{
    OUStringBuffer buf;
    buf.appendAscii( RTL_CONSTASCII_STRINGPARAM( "/SERVICES/" ) );
    buf.append( serviceName );
    return retrieveAsciiValueList( m_xRegistry, buf.makeStringAndClear() );
}

/**
 * Create a service provider from the registry
 */
Reference<XInterface > ORegistryServiceManager::loadWithServiceName(
    const OUString& serviceName, Reference< XComponentContext > const & xContext )
{
    Sequence<OUString> implEntries = getFromServiceName( serviceName );
    for (sal_Int32 i = 0; i < implEntries.getLength(); i++)
    {
        Reference< XInterface > x(
            loadWithImplementationName( implEntries.getConstArray()[i], xContext ) );
        if (x.is())
            return x;
    }

    return Reference<XSingleServiceFactory >();
}

/**
 * Return a sequence of all service names from the registry.
 */
void ORegistryServiceManager::fillAllNamesFromRegistry( HashSet_OWString & rSet )
{
    Reference<XRegistryKey > xRootKey = getRootKey();
    if( !xRootKey.is() )
        return;

    try
    {
        Reference<XRegistryKey > xServicesKey = xRootKey->openKey(
            OUString( RTL_CONSTASCII_USTRINGPARAM("SERVICES") ) );
        // root + /Services + /
        if( xServicesKey.is() )
        {
            sal_Int32 nPrefix = xServicesKey->getKeyName().getLength() +1;
            Sequence<Reference<XRegistryKey > > aKeys = xServicesKey->openKeys();
            for( sal_Int32 i = 0; i < aKeys.getLength(); i++ )
                rSet.insert( aKeys.getConstArray()[i]->getKeyName().copy( nPrefix ) );
        }
    }
    catch (InvalidRegistryException &)
    {
    }
}

// XInitialization
void ORegistryServiceManager::initialize(const Sequence< Any >& Arguments)
    throw(::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException)
{
    check_undisposed();
    MutexGuard aGuard( m_mutex );
    if (Arguments.getLength() > 0)
    {
        m_xRootKey.clear();
        Arguments[ 0 ] >>= m_xRegistry;
    }
#if OSL_DEBUG_LEVEL > 0
    // to find all bootstrapping processes to be fixed...
    OSL_ENSURE( !m_init, "### second init of service manager instance!" );
    m_init = true;
#endif
}

// XMultiServiceFactory, XContentEnumeration
Sequence< OUString > ORegistryServiceManager::getAvailableServiceNames()
    throw(::com::sun::star::uno::RuntimeException)
{
    check_undisposed();
    MutexGuard aGuard( m_mutex );
    // all names
    HashSet_OWString aNameSet;

    // all names from the registry
    fillAllNamesFromRegistry( aNameSet );

    return OServiceManager::getAvailableServiceNames( aNameSet );
}

// XServiceInfo
Sequence< OUString > ORegistryServiceManager::getSupportedServiceNames()
    throw(::com::sun::star::uno::RuntimeException)
{
    check_undisposed();
    return regsmgr_getSupportedServiceNames();
}


// OServiceManager
Sequence< Reference< XInterface > > ORegistryServiceManager::queryServiceFactories(
    const OUString& aServiceName, Reference< XComponentContext > const & xContext )
{
    Sequence< Reference< XInterface > > ret(
        OServiceManager::queryServiceFactories( aServiceName, xContext ) );
    if (ret.getLength())
    {
        return ret;
    }
    else
    {
        MutexGuard aGuard( m_mutex );
        Reference< XInterface > x( loadWithServiceName( aServiceName, xContext ) );
        if (! x.is())
            x = loadWithImplementationName( aServiceName, xContext );
        return Sequence< Reference< XInterface > >( &x, 1 );
    }
}

// XContentEnumerationAccess
Reference<XEnumeration > ORegistryServiceManager::createContentEnumeration(
    const OUString& aServiceName, Reference< XComponentContext > const & xContext )
    throw(::com::sun::star::uno::RuntimeException)
{
    check_undisposed();
    MutexGuard aGuard( ((ORegistryServiceManager *)this)->m_mutex );
    // get all implementation names registered under this service name from the registry
    Sequence<OUString> aImpls = ((ORegistryServiceManager *)this)->getFromServiceName( aServiceName );
    // load and insert all factories specified by the registry
    sal_Int32 i;
    OUString aImplName;
    for( i = 0; i < aImpls.getLength(); i++ )
    {
        aImplName = aImpls.getConstArray()[i];
        if ( !haveFactoryWithThisImplementation(aImplName) )
        {
            loadWithImplementationName( aImplName, xContext );
        }
    }
    // call the superclass to enumerate all contents
    return OServiceManager::createContentEnumeration( aServiceName, xContext );
}
Reference<XEnumeration > ORegistryServiceManager::createContentEnumeration(
    const OUString& aServiceName )
    throw(::com::sun::star::uno::RuntimeException)
{
    return createContentEnumeration( aServiceName, m_xContext );
}

// OServiceManager
Reference<XPropertySetInfo > ORegistryServiceManager::getPropertySetInfo()
    throw(::com::sun::star::uno::RuntimeException)
{
    check_undisposed();
    if (! m_xPropertyInfo.is())
    {
        Sequence< beans::Property > seq( 2 );
        seq[ 0 ] = beans::Property(
            OUSTR("DefaultContext"), -1, ::getCppuType( &m_xContext ), 0 );
        seq[ 1 ] = beans::Property(
            OUSTR("Registry"), -1, ::getCppuType( &m_xRegistry ),
            beans::PropertyAttribute::READONLY );
        Reference< beans::XPropertySetInfo > xInfo( new PropertySetInfo_Impl( seq ) );

        MutexGuard aGuard( m_mutex );
        if (! m_xPropertyInfo.is())
        {
            m_xPropertyInfo = xInfo;
        }
    }
    return m_xPropertyInfo;
}

Any ORegistryServiceManager::getPropertyValue(const OUString& PropertyName)
    throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
{
    check_undisposed();
    if (PropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("Registry") ))
    {
        MutexGuard aGuard( m_mutex );
        if( m_xRegistry.is() )
            return makeAny( m_xRegistry );
        else
            return Any();
    }
    return OServiceManager::getPropertyValue( PropertyName );
}


/**
 * Create the ServiceManager
 */
static Reference<XInterface > SAL_CALL OServiceManager_CreateInstance(
    const Reference< XComponentContext > & xContext )
{
    return Reference<XInterface >(
        SAL_STATIC_CAST(
            XInterface *, SAL_STATIC_CAST(
                OWeakObject *, new OServiceManager( xContext ) ) ) );
}

/**
 * Create the ServiceManager
 */
static Reference<XInterface > SAL_CALL ORegistryServiceManager_CreateInstance(
    const Reference< XComponentContext > & xContext )
    throw(::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException)
{
    return Reference<XInterface >(
        SAL_STATIC_CAST(
            XInterface *, SAL_STATIC_CAST(
                OWeakObject *, new ORegistryServiceManager( xContext ) ) ) );
}

/* This is the listener function used by the service manager in order
to implement the unloading mechanism, id is the this pointer of the
service manager instances. On notification, that is the function is being called
by rtl_unloadUnusedModules, the cached factroies are being removed from the
service manager ( except manually inserted factories).
*/
extern "C" void SAL_CALL smgrUnloadingListener(void* id)
{
    stoc_smgr::OServiceManager* pMgr= reinterpret_cast<stoc_smgr::OServiceManager*>( id);
      pMgr->onUnloadingNotify();
}

} // namespace





//##################################################################################################
//##################################################################################################
//##################################################################################################

using namespace stoc_smgr;

static struct ImplementationEntry g_entries[] =
{
    {
        OServiceManager_CreateInstance, smgr_getImplementationName,
        smgr_getSupportedServiceNames, createSingleComponentFactory,
        &g_moduleCount.modCnt , 0
    },
    {
        ORegistryServiceManager_CreateInstance, regsmgr_getImplementationName,
        regsmgr_getSupportedServiceNames, createSingleComponentFactory,
        &g_moduleCount.modCnt , 0
    },
    {
        OServiceManagerWrapper_CreateInstance, smgr_wrapper_getImplementationName,
        smgr_wrapper_getSupportedServiceNames, createSingleComponentFactory,
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
    return component_writeInfoHelper( pServiceManager, pRegistryKey, g_entries );
}
//==================================================================================================
void * SAL_CALL component_getFactory(
    const sal_Char * pImplName, void * pServiceManager, void * pRegistryKey )
{
    return component_getFactoryHelper( pImplName, pServiceManager, pRegistryKey , g_entries );
}

} //extern "C"

