/*************************************************************************
 *
 *  $RCSfile: component_context.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: dbo $ $Date: 2001-05-31 12:46:42 $
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
#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif

#ifndef _CPPUHELPER_COMPBASE1_HXX
#include <cppuhelper/compbase1.hxx>
#endif
#ifndef _CPPUHELPER_COMPONENT_CONTEXT_HXX
#include <cppuhelper/component_context.hxx>
#endif

#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/registry/XSimpleRegistry.hpp>

#include <hash_map>

using namespace ::osl;
using namespace ::rtl;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star;


namespace cppu
{

//==================================================================================================
struct MutexHolder
{
    Mutex m_mutex;
};
//==================================================================================================
class ComponentContext
    : public MutexHolder
    , public WeakComponentImplHelper1< XComponentContext >
{
    Reference< XComponentContext > m_xDelegate;
    
    struct ContextEntry
    {
        bool bLateInitService;
        Any value;
        
        inline ContextEntry( bool bLateInitService_, Any const & value_ )
            : bLateInitService( bLateInitService_ )
            , value( value_ )
            {}
    };
    typedef ::std::hash_map< OUString, ContextEntry *, OUStringHash > t_map;
    t_map m_map;
    
    Reference< lang::XMultiComponentFactory > m_xSMgr;
    
protected:
    virtual void SAL_CALL disposing();
    
public:
    ComponentContext(
        ContextEntry_Init const * pEntries, sal_Int32 nEntries,
        Reference< XComponentContext > const & xDelegate )
        SAL_THROW( () );
    ComponentContext(
        ContextEntry_Init const * pEntries, sal_Int32 nEntries,
        Reference< registry::XSimpleRegistry > const & xRegistry )
        SAL_THROW( () );
    virtual ~ComponentContext();

    // XComponentContext
    virtual Any SAL_CALL getValueByName( OUString const & rName )
        throw (RuntimeException);
    virtual Reference< lang::XMultiComponentFactory > SAL_CALL getServiceManager()
        throw (RuntimeException);
};
//__________________________________________________________________________________________________
Any ComponentContext::getValueByName( OUString const & rName )
    throw (RuntimeException)
{
    /** map is anytime untouched, if an uninit value will be inited, synch is done on mutex.
     */
    
    t_map::const_iterator const iFind( m_map.find( rName ) );
    if (iFind != m_map.end())
    {
        ContextEntry * pEntry = iFind->second;
        
        if (pEntry->bLateInitService)
        {
            try
            {
                Reference< XInterface > xInstance;
                
                Reference< lang::XSingleComponentFactory > xFac;
                if (pEntry->value >>= xFac) // try via factory
                {
                    xInstance = xFac->createInstanceWithContext( this );
                }
                else // optionally service name
                {
                    OUString aServiceName;
                    if ((pEntry->value >>= aServiceName) && aServiceName.getLength())
                    {
                        if (m_xSMgr.is())
                        {
                            xInstance = m_xSMgr->createInstanceWithContext( aServiceName, this );
                        }
#ifdef _DEBUG
                        else
                        {
                            OSL_TRACE( "### no service manager given for instanciating singletons!" );
                        }
#endif
                    }
                }
                
                if (xInstance.is())
                {
                    ClearableMutexGuard aGuard( m_mutex );
                    if (! pEntry->bLateInitService) // inited in the meantime?
                    {
                        aGuard.clear();
                        // service has entered the context in the meantime
                        // => try to dispose this one
                        Reference< lang::XComponent > xComp( xInstance, UNO_QUERY );
                        if (xComp.is())
                        {
                            xComp->dispose();
                        }
                    }
                    else
                    {
                        pEntry->value.setValue( &xInstance, ::getCppuType( &xInstance ) );
                        pEntry->bLateInitService = false;
                    }
                    
                    return pEntry->value;
                }
                else
                {
                    OString aStr( OUStringToOString( rName, RTL_TEXTENCODING_ASCII_US ) );
                    OSL_TRACE( "### cannot raise singleton: $s\n", aStr.getStr() );
                }
            }
            catch (Exception & rExc)
            {
#ifdef _DEBUG
                OString aStr( OUStringToOString( rName, RTL_TEXTENCODING_ASCII_US ) );
                OString aStr2( OUStringToOString( rExc.Message, RTL_TEXTENCODING_ASCII_US ) );
                OSL_TRACE( "### exception occured raising singleton %s: %s\n", aStr.getStr(), aStr2.getStr() );
#endif
            }
        }
        else
        {
            return pEntry->value;
        }
        
        return Any(); // error occured
    }
    
    if (m_xDelegate.is())
    {
        return m_xDelegate->getValueByName( rName );
    }
    
    return Any();
}
//__________________________________________________________________________________________________
Reference< lang::XMultiComponentFactory > ComponentContext::getServiceManager()
    throw (RuntimeException)
{
    return m_xSMgr;
}
//__________________________________________________________________________________________________
ComponentContext::~ComponentContext()
{
}
//__________________________________________________________________________________________________
void ComponentContext::disposing()
{
    // dispose all context objects
    t_map::const_iterator iPos( m_map.begin() );
    for ( ; iPos != m_map.end(); ++iPos )
    {
        ContextEntry * pEntry = iPos->second;
        Reference< lang::XComponent > xComp;
        
        if (pEntry->bLateInitService)
        {
            // may be in late init
            MutexGuard aGuard( m_mutex );
            pEntry->value >>= xComp;
        }
        else
        {
            pEntry->value >>= xComp;
        }
        
        if (xComp.is())
        {
            xComp->dispose();
        }
    }
    
    // dispose service manager
    Reference< lang::XComponent > xComp( m_xSMgr, UNO_QUERY );
    if (xComp.is())
    {
        xComp->dispose();
    }
    
    // everything is disposed, hopefully nobody accesses the context anymore...
    for ( iPos = m_map.begin(); iPos != m_map.end(); ++iPos )
    {
        delete iPos->second;
    }
    m_map.clear();
}
//__________________________________________________________________________________________________
ComponentContext::ComponentContext(
    ContextEntry_Init const * pEntries, sal_Int32 nEntries,
    Reference< XComponentContext > const & xDelegate )
    SAL_THROW( () )
    : WeakComponentImplHelper1< XComponentContext >( m_mutex )
    , m_xDelegate( xDelegate )
{
    while (nEntries--)
    {
        ContextEntry_Init const & rEntry = pEntries[ nEntries ];
        
        if (rEntry.name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.lang.ServiceManager") ))
        {
            if (rEntry.value >>= m_xSMgr)
                continue;
        }
        m_map[ rEntry.name ] = new ContextEntry( rEntry.bLateInitService, rEntry.value );
    }
    if (!m_xSMgr.is() && m_xDelegate.is())
    {
        m_xSMgr = m_xDelegate->getServiceManager();
    }
}
//__________________________________________________________________________________________________
ComponentContext::ComponentContext(
    ContextEntry_Init const * pEntries, sal_Int32 nEntries,
    Reference< registry::XSimpleRegistry > const & xRegistry )
    SAL_THROW( () )
    : WeakComponentImplHelper1< XComponentContext >( m_mutex )
{
    // inited values
    while (nEntries--)
    {
        ContextEntry_Init const & rEntry = pEntries[ nEntries ];
        
        if (rEntry.name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.lang.ServiceManager") ))
        {
            if (rEntry.value >>= m_xSMgr)
                continue;
        }
        m_map[ rEntry.name ] = new ContextEntry( rEntry.bLateInitService, rEntry.value );
    }
    
    // late init singleton services
    if (xRegistry.is())
    {
        Reference< registry::XRegistryKey > xKey( xRegistry->getRootKey() );
        if (xKey.is())
        {
            xKey = xKey->openKey( OUString( RTL_CONSTASCII_USTRINGPARAM("/SINGLETONS") ) );
            if (xKey.is())
            {
                Sequence< Reference< registry::XRegistryKey > > keys( xKey->openKeys() );
                Reference< registry::XRegistryKey > const * pKeys = keys.getConstArray();
                for ( sal_Int32 nPos = keys.getLength(); nPos--; )
                {
                    try
                    {
                        Reference< registry::XRegistryKey > const & xKey = pKeys[ nPos ];
                        m_map[ xKey->getKeyName().copy( sizeof("/SINGLETONS") /* -\0 +'/' */ ) ] =
                            new ContextEntry( true, makeAny( xKey->getStringValue() ) );
                    }
                    catch (Exception & rExc)
                    {
#ifdef _DEBUG
                        OString aStr( OUStringToOString( xKey->getKeyName().copy( 11 ), RTL_TEXTENCODING_ASCII_US ) );
                        OString aStr2( OUStringToOString( rExc.Message, RTL_TEXTENCODING_ASCII_US ) );
                        OSL_TRACE( "### failed reading singleton [%s] service name from registry: %s", aStr.getStr(), aStr2.getStr() );
#endif
                    }
                }
            }
        }
    }
}

//==================================================================================================
Reference< XComponentContext > SAL_CALL createInitialComponentContext(
    ContextEntry_Init const * pEntries, sal_Int32 nEntries,
    Reference< registry::XSimpleRegistry > const & xRegistry )
    SAL_THROW( () )
{
    return new ComponentContext( pEntries, nEntries, xRegistry );
}

//##################################################################################################
Reference< XComponentContext > SAL_CALL createComponentContext(
    ContextEntry_Init const * pEntries, sal_Int32 nEntries,
    Reference< XComponentContext > const & xDelegate )
    SAL_THROW( () )
{
    return new ComponentContext( pEntries, nEntries, xDelegate );
}

}
