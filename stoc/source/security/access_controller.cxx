/*************************************************************************
 *
 *  $RCSfile: access_controller.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: dbo $ $Date: 2002-04-19 15:16:45 $
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

#include <vector>
#include <memory>

#include <osl/diagnose.h>
#include <osl/interlck.h>
#include <osl/mutex.hxx>
#include <osl/thread.hxx>

#include <rtl/ustrbuf.hxx>
#include <rtl/string.hxx>

#include <uno/current_context.h>

#include <cppuhelper/implbase1.hxx>
#include <cppuhelper/compbase3.hxx>
#include <cppuhelper/factory.hxx>
#include <cppuhelper/implementationentry.hxx>

#include <com/sun/star/uno/XCurrentContext.hpp>
#include <com/sun/star/uno/DeploymentException.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/security/XAccessController.hpp>
#include <com/sun/star/security/XPolicy.hpp>

#include "lru_cache.h"
#include "permissions.h"

#define OUSTR(x) ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(x) )
#define SERVICE_NAME "com.sun.star.security.AccessController"
#define IMPL_NAME "com.sun.star.security.comp.stoc.AccessController"
#define USER_CREDS "access-control.user-credentials"


using namespace ::std;
using namespace ::osl;
using namespace ::cppu;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using ::rtl::OUString;
using ::rtl::OUStringBuffer;
using ::rtl::OString;

namespace stoc_sec
{

// static stuff initialized when loading lib
static OUString s_envType = OUSTR(CPPU_CURRENT_LANGUAGE_BINDING_NAME);
static OUString s_implName = OUSTR(IMPL_NAME);
static OUString s_serviceName = OUSTR(SERVICE_NAME);
static OUString s_acRestriction = OUSTR("access-control.restriction");

static Sequence< OUString > s_serviceNames = Sequence< OUString >( &s_serviceName, 1 );
::rtl_StandardModuleCount s_moduleCount = MODULE_COUNT_INIT;

//##################################################################################################

/** ac context intersects permissions of two ac contexts
*/
class acc_Intersection
    : public WeakImplHelper1< security::XAccessControlContext >
{
    Reference< security::XAccessControlContext > m_x1, m_x2;
    
    inline acc_Intersection(
        Reference< security::XAccessControlContext > const & x1,
        Reference< security::XAccessControlContext > const & x2 )
        SAL_THROW( () );
    
public:
    virtual ~acc_Intersection()
        SAL_THROW( () );
    
    static inline Reference< security::XAccessControlContext > create(
        Reference< security::XAccessControlContext > const & x1,
        Reference< security::XAccessControlContext > const & x2 )
        SAL_THROW( () );
    
    // XAccessControlContext impl
    virtual void SAL_CALL checkPermission(
        Any const & perm )
        throw (RuntimeException);
};
//__________________________________________________________________________________________________
inline acc_Intersection::acc_Intersection(
    Reference< security::XAccessControlContext > const & x1,
    Reference< security::XAccessControlContext > const & x2 )
    SAL_THROW( () )
    : m_x1( x1 )
    , m_x2( x2 )
{
    s_moduleCount.modCnt.acquire( &s_moduleCount.modCnt );
}
//__________________________________________________________________________________________________
acc_Intersection::~acc_Intersection()
    SAL_THROW( () )
{
    s_moduleCount.modCnt.release( &s_moduleCount.modCnt );
}
//--------------------------------------------------------------------------------------------------
inline Reference< security::XAccessControlContext > acc_Intersection::create(
    Reference< security::XAccessControlContext > const & x1,
    Reference< security::XAccessControlContext > const & x2 )
    SAL_THROW( () )
{
    if (! x1.is())
        return x2;
    if (! x2.is())
        return x1;
    return new acc_Intersection( x1, x2 );
}
//__________________________________________________________________________________________________
void acc_Intersection::checkPermission(
    Any const & perm )
    throw (RuntimeException)
{
    m_x1->checkPermission( perm );
    m_x2->checkPermission( perm );
}

/** ac context unifies permissions of two ac contexts
*/
class acc_Union
    : public WeakImplHelper1< security::XAccessControlContext >
{
    Reference< security::XAccessControlContext > m_x1, m_x2;
    
    inline acc_Union(
        Reference< security::XAccessControlContext > const & x1,
        Reference< security::XAccessControlContext > const & x2 )
        SAL_THROW( () );
    
public:
    virtual ~acc_Union()
        SAL_THROW( () );
    
    static inline Reference< security::XAccessControlContext > create(
        Reference< security::XAccessControlContext > const & x1,
        Reference< security::XAccessControlContext > const & x2 )
        SAL_THROW( () );
    
    // XAccessControlContext impl
    virtual void SAL_CALL checkPermission(
        Any const & perm )
        throw (RuntimeException);
};
//__________________________________________________________________________________________________
inline acc_Union::acc_Union(
    Reference< security::XAccessControlContext > const & x1,
    Reference< security::XAccessControlContext > const & x2 )
    SAL_THROW( () )
    : m_x1( x1 )
    , m_x2( x2 )
{
    s_moduleCount.modCnt.acquire( &s_moduleCount.modCnt );
}
//__________________________________________________________________________________________________
acc_Union::~acc_Union()
    SAL_THROW( () )
{
    s_moduleCount.modCnt.release( &s_moduleCount.modCnt );
}
//--------------------------------------------------------------------------------------------------
inline Reference< security::XAccessControlContext > acc_Union::create(
    Reference< security::XAccessControlContext > const & x1,
    Reference< security::XAccessControlContext > const & x2 )
    SAL_THROW( () )
{
    if (! x1.is())
        return Reference< security::XAccessControlContext >(); // unrestricted
    if (! x2.is())
        return Reference< security::XAccessControlContext >(); // unrestricted
    return new acc_Union( x1, x2 );
}
//__________________________________________________________________________________________________
void acc_Union::checkPermission(
    Any const & perm )
    throw (RuntimeException)
{
    try
    {
        m_x1->checkPermission( perm );
    }
    catch (security::AccessControlException &)
    {
        m_x2->checkPermission( perm );
    }
}

/** ac context doing permission checks on static permissions
*/
class acc_Policy
    : public WeakImplHelper1< security::XAccessControlContext >
{
    PermissionCollection m_permissions;
    
public:
    inline acc_Policy(
        PermissionCollection const & permissions )
        SAL_THROW( () );
    virtual ~acc_Policy()
        SAL_THROW( () );
    
    // XAccessControlContext impl
    virtual void SAL_CALL checkPermission(
        Any const & perm )
        throw (RuntimeException);
};
//__________________________________________________________________________________________________
inline acc_Policy::acc_Policy(
    PermissionCollection const & permissions )
    SAL_THROW( () )
    : m_permissions( permissions )
{
    s_moduleCount.modCnt.acquire( &s_moduleCount.modCnt );
}
//__________________________________________________________________________________________________
acc_Policy::~acc_Policy()
    SAL_THROW( () )
{
    s_moduleCount.modCnt.release( &s_moduleCount.modCnt );
}
//__________________________________________________________________________________________________
void acc_Policy::checkPermission(
    Any const & perm )
    throw (RuntimeException)
{
    m_permissions.checkPermission( perm );
}

/** current context overriding dynamic ac restriction
*/
class acc_CurrentContext
    : public ImplHelper1< XCurrentContext >
{
    oslInterlockedCount m_refcount;
    
    Reference< XCurrentContext > m_xDelegate;
    Any m_restriction;
    
public:
    inline acc_CurrentContext(
        Reference< XCurrentContext > const & xDelegate,
        Reference< security::XAccessControlContext > const & xRestriction )
        SAL_THROW( () );
    virtual ~acc_CurrentContext() SAL_THROW( () );
    
    // XInterface impl
    virtual void SAL_CALL acquire()
        throw ();
    virtual void SAL_CALL release()
        throw ();
    
    // XCurrentContext impl
    virtual Any SAL_CALL getValueByName( OUString const & name )
        throw (RuntimeException);
};
//__________________________________________________________________________________________________
inline acc_CurrentContext::acc_CurrentContext(
    Reference< XCurrentContext > const & xDelegate,
    Reference< security::XAccessControlContext > const & xRestriction )
    SAL_THROW( () )
    : m_refcount( 0 )
    , m_xDelegate( xDelegate )
{
    s_moduleCount.modCnt.acquire( &s_moduleCount.modCnt );
    
    if (xRestriction.is())
    {
        m_restriction = makeAny( xRestriction );
    }
    // return empty any otherwise on getValueByName(), not null interface
}
//__________________________________________________________________________________________________
acc_CurrentContext::~acc_CurrentContext()
    SAL_THROW( () )
{
    s_moduleCount.modCnt.release( &s_moduleCount.modCnt );
}
//__________________________________________________________________________________________________
void acc_CurrentContext::acquire()
    throw ()
{
    ::osl_incrementInterlockedCount( &m_refcount );
}
//__________________________________________________________________________________________________
void acc_CurrentContext::release()
    throw ()
{
    if (! ::osl_decrementInterlockedCount( &m_refcount ))
    {
        delete this;
    }
}
//__________________________________________________________________________________________________
Any acc_CurrentContext::getValueByName( OUString const & name )
    throw (RuntimeException)
{
    if (name.equals( s_acRestriction ))
    {
        return m_restriction;
    }
    else if (m_xDelegate.is())
    {
        return m_xDelegate->getValueByName( name );
    }
    else
    {
        return Any();
    }
}

//##################################################################################################

//--------------------------------------------------------------------------------------------------
static inline void dispose( Reference< XInterface > const & x )
    SAL_THROW( (RuntimeException) )
{
    Reference< lang::XComponent > xComp( x, UNO_QUERY );
    if (xComp.is())
    {
        xComp->dispose();
    }
}
//--------------------------------------------------------------------------------------------------
static inline Reference< security::XAccessControlContext > getDynamicRestriction(
    Reference< XCurrentContext > const & xContext )
    SAL_THROW( (RuntimeException) )
{
    if (xContext.is())
    {
        Any acc( xContext->getValueByName( s_acRestriction ) );
        if (typelib_TypeClass_INTERFACE == acc.pType->eTypeClass)
        {
            // avoid ref-counting
            OUString const & typeName =
                *reinterpret_cast< OUString const * >( &acc.pType->pTypeName );
            if (typeName.equalsAsciiL(
                    RTL_CONSTASCII_STRINGPARAM("com.sun.star.security.XAccessControlContext") ))
            {
                return Reference< security::XAccessControlContext >(
                    *reinterpret_cast< security::XAccessControlContext ** const >( acc.pData ) );
            }
            else // try to query
            {
                return Reference< security::XAccessControlContext >::query(
                    *reinterpret_cast< XInterface ** const >( acc.pData ) );
            }
        }
    }
    return Reference< security::XAccessControlContext >();
}
//==================================================================================================
class cc_reset
{
    void * m_cc;
public:
    inline cc_reset( void * cc ) SAL_THROW( () )
        : m_cc( cc ) {}
    inline ~cc_reset() SAL_THROW( () )
        { ::uno_setCurrentContext( m_cc, s_envType.pData, 0 ); }
};

//##################################################################################################

struct MutexHolder
{
    Mutex m_mutex;
};
typedef WeakComponentImplHelper3<
    security::XAccessController, lang::XServiceInfo, lang::XInitialization > t_helper;

//==================================================================================================
class AccessController
    : public MutexHolder
    , public t_helper
{
    Reference< XComponentContext > m_xComponentContext;
    
    Reference< security::XPolicy > m_xPolicy;
    Reference< security::XPolicy > const & getPolicy()
        SAL_THROW( (RuntimeException) );
    
    // mode
    enum { OFF, ON, DYNAMIC_ONLY, SINGLE_USER, SINGLE_DEFAULT_USER } m_mode;
    
    PermissionCollection m_defaultPermissions;
    // for single-user mode
    PermissionCollection m_singleUserPermissions;
    OUString m_singleUserId;
    bool m_defaultPerm_init;
    bool m_singleUser_init;
    // for multi-user mode
    lru_cache< OUString, PermissionCollection, ::rtl::OUStringHash, equal_to< OUString > >
        m_user2permissions;
    
    ThreadData m_rec;
    typedef vector< pair< OUString, Any > > t_rec_vec;
    inline void clearPostPoned() SAL_THROW( () );
    void checkAndClearPostPoned() SAL_THROW( (RuntimeException) );
    
    PermissionCollection getEffectivePermissions(
        Reference< XCurrentContext > const & xContext,
        Any const & demanded_perm )
        SAL_THROW( (RuntimeException) );
    
protected:
    virtual void SAL_CALL disposing();
    
public:
    AccessController( Reference< XComponentContext > const & xComponentContext )
        SAL_THROW( (RuntimeException) );
    virtual ~AccessController()
        SAL_THROW( () );
    
    //  XInitialization impl
    virtual void SAL_CALL initialize(
        Sequence< Any > const & arguments )
        throw (Exception);
    
    // XAccessController impl
    virtual void SAL_CALL checkPermission(
        Any const & perm )
        throw (RuntimeException);
    virtual Any SAL_CALL doRestricted(
        Reference< security::XAction > const & xAction,
        Reference< security::XAccessControlContext > const & xRestriction )
        throw (Exception);
    virtual Any SAL_CALL doPrivileged(
        Reference< security::XAction > const & xAction,
        Reference< security::XAccessControlContext > const & xRestriction )
        throw (Exception);
    virtual Reference< security::XAccessControlContext > SAL_CALL getContext()
        throw (RuntimeException);
    
    // XServiceInfo impl
    virtual OUString SAL_CALL getImplementationName()
        throw (RuntimeException);
    virtual sal_Bool SAL_CALL supportsService( OUString const & serviceName )
        throw (RuntimeException);
    virtual Sequence< OUString > SAL_CALL getSupportedServiceNames()
        throw (RuntimeException);
};
//__________________________________________________________________________________________________
AccessController::AccessController( Reference< XComponentContext > const & xComponentContext )
    SAL_THROW( (RuntimeException) )
    : t_helper( m_mutex )
    , m_xComponentContext( xComponentContext )
    , m_mode( ON ) // default
    , m_defaultPerm_init( false )
    , m_singleUser_init( false )
    , m_rec( 0 )
{
    s_moduleCount.modCnt.acquire( &s_moduleCount.modCnt );
    
    OUString mode;
    if (m_xComponentContext->getValueByName( OUSTR("/services/" SERVICE_NAME "/mode") ) >>= mode)
    {
        if (mode.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("off") ))
        {
            m_mode = OFF;
        }
        else if (mode.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("on") ))
        {
            m_mode = ON;
        }
        else if (mode.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("dynamic-only") ))
        {
            m_mode = DYNAMIC_ONLY;
        }
        else if (mode.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("single-user") ))
        {
            m_xComponentContext->getValueByName(
                OUSTR("/services/" SERVICE_NAME "/single-user-id") ) >>= m_singleUserId;
            if (! m_singleUserId.getLength())
            {
                throw RuntimeException(
                    OUSTR("expected a user id in component context entry "
                          "\"/services/" SERVICE_NAME "/single-user-id\"!"),
                    (OWeakObject *)this );
            }
            m_mode = SINGLE_USER;
        }
        else if (mode.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("single-default-user") ))
        {
            m_mode = SINGLE_DEFAULT_USER;
        }
    }
    
    // switch on caching for DYNAMIC_ONLY and ON (sharable multi-user process)
    if (ON == m_mode || DYNAMIC_ONLY == m_mode)
    {
        sal_Int32 cacheSize; // multi-user cache size
        if (! (m_xComponentContext->getValueByName(
            OUSTR("/services/" SERVICE_NAME "/user-cache-size") ) >>= cacheSize))
        {
            cacheSize = 128; // reasonable default?
        }
#ifdef __CACHE_DIAGNOSE
        cacheSize = 2;
#endif
        m_user2permissions.setSize( cacheSize );
    }
}
//__________________________________________________________________________________________________
AccessController::~AccessController()
    SAL_THROW( () )
{
    s_moduleCount.modCnt.release( &s_moduleCount.modCnt );
}
//__________________________________________________________________________________________________
void AccessController::disposing()
{
    m_mode = OFF; // avoid checks from now on xxx todo review/ better DYNAMIC_ONLY?
    m_xPolicy.clear();
    m_xComponentContext.clear();
}

// XInitialization impl
//__________________________________________________________________________________________________
void AccessController::initialize(
    Sequence< Any > const & arguments )
    throw (Exception)
{
    // xxx todo: review for forking
    // portal forking hack: re-initialize for another user-id
    if (SINGLE_USER != m_mode) // only if in single-user mode
    {
        throw RuntimeException(
            OUSTR("invalid call: ac must be in \"single-user\" mode!"), (OWeakObject *)this );
    }    
    OUString userId;
    arguments[ 0 ] >>= userId;
    if (! userId.getLength())
    {
        throw RuntimeException(
            OUSTR("expected a user-id as first argument!"), (OWeakObject *)this );
    }
    // assured that no sync is necessary: no check happens at this forking time
    m_singleUserId = userId;
    m_singleUser_init = false;
}

//__________________________________________________________________________________________________
Reference< security::XPolicy > const & AccessController::getPolicy()
    SAL_THROW( (RuntimeException) )
{
    // get policy singleton
    if (! m_xPolicy.is())
    {
        Reference< security::XPolicy > xPolicy;
        m_xComponentContext->getValueByName(
            OUSTR("/singletons/com.sun.star.security.thePolicy") ) >>= xPolicy;
        if (xPolicy.is())
        {
            MutexGuard guard( m_mutex );
            if (! m_xPolicy.is())
            {
                m_xPolicy = xPolicy;
            }
        }
        else
        {
            throw SecurityException(
                OUSTR("cannot get policy singleton!"), (OWeakObject *)this );
        }
    }
    return m_xPolicy;
}

#ifdef __DIAGNOSE
static void dumpPermissions(
    PermissionCollection const & collection, OUString const & userId = OUString() ) SAL_THROW( () )
{
    OUStringBuffer buf( 48 );
    if (userId.getLength())
    {
        buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("> dumping permissions of user \"") );
        buf.append( userId );
        buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("\":") );
    }
    else
    {
        buf.appendAscii(
            RTL_CONSTASCII_STRINGPARAM("> dumping default permissions:") );
    }
    OString str( ::rtl::OUStringToOString( buf.makeStringAndClear(), RTL_TEXTENCODING_ASCII_US ) );
    OSL_TRACE( str.getStr() );
    Sequence< OUString > permissions( collection.toStrings() );
    OUString const * p = permissions.getConstArray();
    for ( sal_Int32 nPos = 0; nPos < permissions.getLength(); ++nPos )
    {
        OString str( ::rtl::OUStringToOString( p[ nPos ], RTL_TEXTENCODING_ASCII_US ) );
        OSL_TRACE( str.getStr() );
    }
    OSL_TRACE( "> permission dump done" );
}
#endif


//__________________________________________________________________________________________________
inline void AccessController::clearPostPoned() SAL_THROW( () )
{
    delete reinterpret_cast< t_rec_vec * >( m_rec.getData() );
    m_rec.setData( 0 );
}
//__________________________________________________________________________________________________
void AccessController::checkAndClearPostPoned() SAL_THROW( (RuntimeException) )
{
    // check postponed permissions
    auto_ptr< t_rec_vec > rec( reinterpret_cast< t_rec_vec * >( m_rec.getData() ) );
    m_rec.setData( 0 ); // takeover ownership
    OSL_ASSERT( rec.get() );
    if (rec.get())
    {
        t_rec_vec const & vec = *rec.get();
        switch (m_mode)
        {
        case SINGLE_USER:
        {
            OSL_ASSERT( m_singleUser_init );
            for ( size_t nPos = 0; nPos < vec.size(); ++nPos )
            {
                pair< OUString, Any > const & p = vec[ nPos ];
                OSL_ASSERT( m_singleUserId.equals( p.first ) );
                m_singleUserPermissions.checkPermission( p.second );
            }
            break;
        }
        case SINGLE_DEFAULT_USER:
        {
            OSL_ASSERT( m_defaultPerm_init );
            for ( size_t nPos = 0; nPos < vec.size(); ++nPos )
            {
                pair< OUString, Any > const & p = vec[ nPos ];
                OSL_ASSERT( !p.first.getLength() ); // default-user
                m_defaultPermissions.checkPermission( p.second );
            }
            break;
        }
        case ON:
        {
            for ( size_t nPos = 0; nPos < vec.size(); ++nPos )
            {
                pair< OUString, Any > const & p = vec[ nPos ];
                PermissionCollection const * pPermissions;
                // lookup policy for user
                {
                    MutexGuard guard( m_mutex );
                    pPermissions = m_user2permissions.lookup( p.first );
                }
                OSL_ASSERT( pPermissions );
                if (pPermissions)
                {
                    pPermissions->checkPermission( p.second );
                }
            }
            break;
        }
        default:
            OSL_ENSURE( 0, "### this should never be called in this ac mode!" );
            break;
        }
    }
}
//__________________________________________________________________________________________________
/** this is the only function calling the policy singleton and thus has to take care
    of recurring calls!

    @param demanded_perm (if not empty) is the demanded permission of a checkPermission() call
                         which will be postponed for recurring calls
*/
PermissionCollection AccessController::getEffectivePermissions(
    Reference< XCurrentContext > const & xContext,
    Any const & demanded_perm )
    SAL_THROW( (RuntimeException) )
{
    OUString userId;
    
    switch (m_mode)
    {
    case SINGLE_USER:
    {
        if (m_singleUser_init)
            return m_singleUserPermissions;
        userId = m_singleUserId;
        break;
    }
    case SINGLE_DEFAULT_USER:
    {
        if (m_defaultPerm_init)
            return m_defaultPermissions;
        break;
    }
    case ON:
    {
        if (xContext.is())
        {
            xContext->getValueByName( OUSTR(USER_CREDS ".id") ) >>= userId;
        }
        if (! userId.getLength())
        {
            throw SecurityException(
                OUSTR("cannot determine current user in multi-user ac!"), (OWeakObject *)this );
        }
        
        // lookup policy for user
        MutexGuard guard( m_mutex );
        PermissionCollection const * pPermissions = m_user2permissions.lookup( userId );
        if (pPermissions)
            return *pPermissions;
        break;
    }
    default:
        OSL_ENSURE( 0, "### this should never be called in this ac mode!" );
        return PermissionCollection();
    }
    
    // call on policy
    // iff this is a recurring call for the default user, then grant all permissions
    t_rec_vec * rec = reinterpret_cast< t_rec_vec * >( m_rec.getData() );
    if (rec) // tls entry exists => this is recursive call
    {
        if (demanded_perm.hasValue())
        {
            // enqueue
            rec->push_back( pair< OUString, Any >( userId, demanded_perm ) );
        }
#ifdef __DIAGNOSE
        OUStringBuffer buf( 48 );
        buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("> info: recurring call of user \"") );
        buf.append( userId );
        buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("\"") );
        OString str(
            ::rtl::OUStringToOString( buf.makeStringAndClear(), RTL_TEXTENCODING_ASCII_US ) );
        OSL_TRACE( str.getStr() );
#endif
        return PermissionCollection( new AllPermission() );
    }
    else // no tls
    {
        rec = new t_rec_vec;
        m_rec.setData( rec );
    }
    
    try // calls on API
    {
        // init default permissions
        if (! m_defaultPerm_init)
        {
            PermissionCollection defaultPermissions(
                getPolicy()->getDefaultPermissions() );
            // assign
            MutexGuard guard( m_mutex );
            if (! m_defaultPerm_init)
            {
                m_defaultPermissions = defaultPermissions;
                m_defaultPerm_init = true;
            }
#ifdef __DIAGNOSE
            dumpPermissions( m_defaultPermissions );
#endif
        }
        
        PermissionCollection ret;
        
        // init user permissions
        switch (m_mode)
        {
        case SINGLE_USER:
        {
            ret = PermissionCollection(
                getPolicy()->getPermissions( userId ), m_defaultPermissions );
            {
            // assign
            MutexGuard guard( m_mutex );
            if (m_singleUser_init)
            {
                ret = m_singleUserPermissions;
            }
            else
            {
                m_singleUserPermissions = ret;
                m_singleUser_init = true;
            }
            }
#ifdef __DIAGNOSE
            dumpPermissions( ret, userId );
#endif
            break;
        }
        case SINGLE_DEFAULT_USER:
        {
            ret = m_defaultPermissions;
            break;
        }
        case ON:
        {
            ret = PermissionCollection(
                getPolicy()->getPermissions( userId ), m_defaultPermissions );
            {
            // cache
            MutexGuard guard( m_mutex );
            m_user2permissions.set( userId, ret );
            }
#ifdef __DIAGNOSE
            dumpPermissions( ret, userId );
#endif
            break;
        }
        }
        
        // check postponed
        checkAndClearPostPoned();
        return ret;
    }
    catch (security::AccessControlException & exc) // wrapped into DeploymentException
    {
        clearPostPoned(); // safety: exception could have happened before checking postponed?
        OUStringBuffer buf( 64 );
        buf.appendAscii(
            RTL_CONSTASCII_STRINGPARAM("deployment error (AccessControlException occured): ") );
        buf.append( exc.Message );
        throw DeploymentException( buf.makeStringAndClear(), exc.Context );
    }
    catch (RuntimeException &)
    {
        // dont check postponed, just cleanup
        clearPostPoned();
        delete reinterpret_cast< t_rec_vec * >( m_rec.getData() );
        m_rec.setData( 0 );
        throw;
    }
    catch (Exception &)
    {
        // check postponed permissions first
        // => AccessControlExceptions are errors, user exceptions not!
        checkAndClearPostPoned();
        throw;
    }
    catch (...)
    {
        // dont check postponed, just cleanup
        clearPostPoned();
        throw;
    }
}

// XAccessController impl
//__________________________________________________________________________________________________
void AccessController::checkPermission(
    Any const & perm )
    throw (RuntimeException)
{
    if (rBHelper.bDisposed)
    {
        throw lang::DisposedException(
            OUSTR("checkPermission() call on disposed AccessController!"), (OWeakObject *)this );
    }
    
    if (OFF == m_mode)
        return;
    
    // first dynamic check of ac contexts
    Reference< XCurrentContext > xContext;
    ::uno_getCurrentContext( (void **)&xContext, s_envType.pData, 0 );
    Reference< security::XAccessControlContext > xACC( getDynamicRestriction( xContext ) );
    if (xACC.is())
    {
        xACC->checkPermission( perm );
    }
    
    if (DYNAMIC_ONLY == m_mode)
        return;
    
    // then static check
    getEffectivePermissions( xContext, perm ).checkPermission( perm );
}
//__________________________________________________________________________________________________
Any AccessController::doRestricted(
    Reference< security::XAction > const & xAction,
    Reference< security::XAccessControlContext > const & xRestriction )
    throw (Exception)
{
    if (rBHelper.bDisposed)
    {
        throw lang::DisposedException(
            OUSTR("doRestricted() call on disposed AccessController!"), (OWeakObject *)this );
    }
    
    if (OFF == m_mode) // optimize this way, because no dynamic check will be performed
        return xAction->run();
    
    if (xRestriction.is())
    {
        Reference< XCurrentContext > xContext;
        ::uno_getCurrentContext( (void **)&xContext, s_envType.pData, 0 );
        
        // override restriction
        Reference< XCurrentContext > xNewContext(
            new acc_CurrentContext( xContext, acc_Intersection::create(
                                        xRestriction, getDynamicRestriction( xContext ) ) ) );
        ::uno_setCurrentContext( xNewContext.get(), s_envType.pData, 0 );
        cc_reset reset( xContext.get() );
        return xAction->run();
    }
    else
    {
        return xAction->run();
    }
}
//__________________________________________________________________________________________________
Any AccessController::doPrivileged(
    Reference< security::XAction > const & xAction,
    Reference< security::XAccessControlContext > const & xRestriction )
    throw (Exception)
{
    if (rBHelper.bDisposed)
    {
        throw lang::DisposedException(
            OUSTR("doPrivileged() call on disposed AccessController!"), (OWeakObject *)this );
    }
    
    if (OFF == m_mode) // no dynamic check will be performed
    {
        return xAction->run();
    }
    
    Reference< XCurrentContext > xContext;
    ::uno_getCurrentContext( (void **)&xContext, s_envType.pData, 0 );
    
    Reference< security::XAccessControlContext > xOldRestr(
        getDynamicRestriction( xContext ) );
    
    if (xOldRestr.is()) // previous restriction
    {
        // override restriction
        Reference< XCurrentContext > xNewContext(
            new acc_CurrentContext( xContext, acc_Union::create( xRestriction, xOldRestr ) ) );
        ::uno_setCurrentContext( xNewContext.get(), s_envType.pData, 0 );
        cc_reset reset( xContext.get() );
        return xAction->run();
    }
    else // no previous restriction => never current restriction
    {
        return xAction->run();
    }
}
//__________________________________________________________________________________________________
Reference< security::XAccessControlContext > AccessController::getContext()
    throw (RuntimeException)
{
    if (rBHelper.bDisposed)
    {
        throw lang::DisposedException(
            OUSTR("getContext() call on disposed AccessController!"), (OWeakObject *)this );
    }
    
    if (OFF == m_mode) // optimize this way, because no dynamic check will be performed
    {
        return new acc_Policy( PermissionCollection( new AllPermission() ) );
    }
    
    Reference< XCurrentContext > xContext;
    ::uno_getCurrentContext( (void **)&xContext, s_envType.pData, 0 );
    
    return acc_Intersection::create(
        getDynamicRestriction( xContext ),
        new acc_Policy( getEffectivePermissions( xContext, Any() ) ) );
}

// XServiceInfo impl
//__________________________________________________________________________________________________
OUString AccessController::getImplementationName()
    throw (RuntimeException)
{
    return s_implName;
}
//__________________________________________________________________________________________________
sal_Bool AccessController::supportsService( OUString const & serviceName )
    throw (RuntimeException)
{
    OUString const * pNames = s_serviceNames.getConstArray();
    for ( sal_Int32 nPos = s_serviceNames.getLength(); nPos--; )
    {
        if (serviceName.equals( pNames[ nPos ] ))
        {
            return sal_True;
        }
    }
    return sal_False;
}
//__________________________________________________________________________________________________
Sequence< OUString > AccessController::getSupportedServiceNames()
    throw (RuntimeException)
{
    return s_serviceNames;
}

//##################################################################################################

//--------------------------------------------------------------------------------------------------
static Reference< XInterface > SAL_CALL ac_create(
    Reference< XComponentContext > const & xComponentContext )
    SAL_THROW( (Exception) )
{
    return (OWeakObject *)new AccessController( xComponentContext );
}
//--------------------------------------------------------------------------------------------------
static Sequence< OUString > ac_getSupportedServiceNames() SAL_THROW( () )
{
    return s_serviceNames;
}
//--------------------------------------------------------------------------------------------------
static OUString ac_getImplementationName() SAL_THROW( () )
{
    return s_implName;
}
//--------------------------------------------------------------------------------------------------
Reference< XInterface > SAL_CALL filepolicy_create(
    Reference< XComponentContext > const & xComponentContext )
    SAL_THROW( (Exception) );
//--------------------------------------------------------------------------------------------------
Sequence< OUString > filepolicy_getSupportedServiceNames() SAL_THROW( () );
//--------------------------------------------------------------------------------------------------
OUString filepolicy_getImplementationName() SAL_THROW( () );
//--------------------------------------------------------------------------------------------------
static struct ImplementationEntry s_entries [] =
{
    {
        ac_create, ac_getImplementationName,
        ac_getSupportedServiceNames, createSingleComponentFactory,
        &s_moduleCount.modCnt, 0
    },
    {
        filepolicy_create, filepolicy_getImplementationName,
        filepolicy_getSupportedServiceNames, createSingleComponentFactory,
        &s_moduleCount.modCnt, 0
    },
    { 0, 0, 0, 0, 0, 0 }
};

}

extern "C"
{
//==================================================================================================
sal_Bool SAL_CALL component_canUnload( TimeValue * time )
{
    return ::stoc_sec::s_moduleCount.canUnload( &::stoc_sec::s_moduleCount, time );
}
//==================================================================================================
void SAL_CALL component_getImplementationEnvironment(
    sal_Char const ** ppEnvTypeName, uno_Environment ** ppEnv )
{
    *ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}
//==================================================================================================
sal_Bool SAL_CALL component_writeInfo(
    lang::XMultiServiceFactory * smgr, registry::XRegistryKey * key )
{
    return component_writeInfoHelper( smgr, key, ::stoc_sec::s_entries );
}
//==================================================================================================
void * SAL_CALL component_getFactory(
    sal_Char const * implName, lang::XMultiServiceFactory * smgr, registry::XRegistryKey * key )
{
    return component_getFactoryHelper( implName, smgr, key, ::stoc_sec::s_entries );
}
}
