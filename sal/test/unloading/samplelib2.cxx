
#include <sal/types.h>
#include <osl/time.h>
#include <rtl/ustring.hxx>
#include <uno/environment.h>
#include <cppu/macros.hxx>
#include <cppuhelper/factory.hxx>
#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include <cppuhelper/implbase1.hxx>
#include <rtl/unload.h>
#include <rtl/string.hxx>
#include <rtl/ustrbuf.hxx>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/registry/XRegistryKey.hpp>

using namespace ::com::sun::star::registry;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::rtl;
using namespace ::cppu;

#define IMPLNAME1 "com.sun.star.comp.sal.UnloadingTest21"
#define SERVICENAME1 "com.sun.star.UnloadingTest21"
#define IMPLNAME2 "com.sun.star.comp.sal.UnloadingTest22"
#define SERVICENAME2 "com.sun.star.UnloadingTest22"
#define IMPLNAME3 "com.sun.star.comp.sal.UnloadingTest23"
#define SERVICENAME3 "com.sun.star.UnloadingTest23"

// Unloading Support ----------------------------------------------
rtl_StandardModuleCount globalModuleCount= MODULE_COUNT_INIT;
//rtl_StandardModuleCount globalModuleCount= { {rtl_moduleCount_acquire,rtl_moduleCount_release}, rtl_moduleCount_canUnload,0,{0,0}}; //, 0, {0, 0}};
// Services -------------------------------------------------------
class TestService: public WeakImplHelper1<XServiceInfo>
{
OUString m_implName;
OUString m_serviceName;
public:
    TestService( OUString implName, OUString serviceName);
    ~TestService();
    virtual OUString SAL_CALL getImplementationName(  )  throw (RuntimeException);
    virtual sal_Bool SAL_CALL supportsService( const OUString& ServiceName ) throw (RuntimeException);
    virtual Sequence<OUString > SAL_CALL getSupportedServiceNames(  ) throw (RuntimeException);
};

TestService::TestService( OUString implName, OUString serviceName):
            m_implName(implName),m_serviceName(serviceName)
{	// Library unloading support
    globalModuleCount.modCnt.acquire( &globalModuleCount.modCnt);
}

TestService::~TestService()
{	// Library unloading support
    globalModuleCount.modCnt.release( &globalModuleCount.modCnt);
}

OUString SAL_CALL TestService::getImplementationName(  )  throw (RuntimeException)
{
    return m_implName;
}
sal_Bool SAL_CALL TestService::supportsService( const OUString& ServiceName ) throw (RuntimeException)
{
    return ServiceName.equals( m_serviceName);
}
Sequence<OUString > SAL_CALL TestService::getSupportedServiceNames(  ) throw (RuntimeException)
{
    return Sequence<OUString>( &m_serviceName, 1);
}


// Creator functions for Services -------------------------------------------------
static Reference<XInterface> SAL_CALL test21_createInstance(const Reference<XMultiServiceFactory> & rSMgr)
        throw (RuntimeException)
{
    return Reference<XInterface>( static_cast<XWeak*>( new TestService(
        OUString( RTL_CONSTASCII_USTRINGPARAM( IMPLNAME1)),
        OUString( RTL_CONSTASCII_USTRINGPARAM( SERVICENAME1)) )), UNO_QUERY);
}
static Reference<XInterface> SAL_CALL test22_createInstance(const Reference<XMultiServiceFactory> & rSMgr)
        throw (RuntimeException)
{
    return Reference<XInterface>( static_cast<XWeak*>( new TestService(
        OUString( RTL_CONSTASCII_USTRINGPARAM( IMPLNAME2)),
        OUString( RTL_CONSTASCII_USTRINGPARAM( SERVICENAME2)) )), UNO_QUERY);
}
static Reference<XInterface> SAL_CALL test23_createInstance(const Reference<XMultiServiceFactory> & rSMgr)
        throw (RuntimeException)
{
    return Reference<XInterface>( static_cast<XWeak*>( new TestService(
        OUString( RTL_CONSTASCII_USTRINGPARAM( IMPLNAME3)),
        OUString( RTL_CONSTASCII_USTRINGPARAM( SERVICENAME3)) )), UNO_QUERY);
}


// Standard UNO library interface -------------------------------------------------
extern "C" {
    void SAL_CALL component_getImplementationEnvironment(const sal_Char ** ppEnvTypeName, uno_Environment ** ppEnv){
        *ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
    }

    sal_Bool SAL_CALL component_writeInfo(void * pServiceManager, void * pRegistryKey) throw()
    {
        if (pRegistryKey)
        {
            try
            {
                Reference< XRegistryKey > xNewKey(
                    reinterpret_cast< XRegistryKey * >( pRegistryKey )->createKey(
                        OUString::createFromAscii( "/" IMPLNAME1 "/UNO/SERVICES" ) ) );

                xNewKey->createKey( OUString( RTL_CONSTASCII_USTRINGPARAM( SERVICENAME1)));

                xNewKey= 
                    reinterpret_cast< XRegistryKey * >( pRegistryKey )->createKey(
                        OUString::createFromAscii( "/" IMPLNAME2 "/UNO/SERVICES" ) );

                xNewKey->createKey(OUString( RTL_CONSTASCII_USTRINGPARAM( SERVICENAME2)));
                xNewKey= 
                    reinterpret_cast< XRegistryKey * >( pRegistryKey )->createKey(
                        OUString::createFromAscii( "/" IMPLNAME3 "/UNO/SERVICES" )   );

                xNewKey->createKey(OUString( RTL_CONSTASCII_USTRINGPARAM( SERVICENAME3)));
                
                return sal_True;
            }
            catch (InvalidRegistryException &)
            {
                OSL_ENSURE( sal_False, "### InvalidRegistryException!" );
            }
        }
        return sal_False;
    }

    void * SAL_CALL component_getFactory(const sal_Char * pImplName, void * pServiceManager, void * pRegistryKey) throw() 
    {
        void * pRet = 0;
    

        OUString implname1( RTL_CONSTASCII_USTRINGPARAM( IMPLNAME1) );
        OUString serviceName1( RTL_CONSTASCII_USTRINGPARAM( SERVICENAME1) );
        OUString implname2( RTL_CONSTASCII_USTRINGPARAM( IMPLNAME2) );
        OUString serviceName2( RTL_CONSTASCII_USTRINGPARAM( SERVICENAME2) );
        OUString implname3( RTL_CONSTASCII_USTRINGPARAM( IMPLNAME3) );
        OUString serviceName3( RTL_CONSTASCII_USTRINGPARAM( SERVICENAME3) );

        if (implname1.equals( OUString::createFromAscii(pImplName)))
        {
            Reference<XMultiServiceFactory> mgr= reinterpret_cast<XMultiServiceFactory *>(pServiceManager);
            Reference<XSingleServiceFactory> xFactory( createSingleFactory(
                reinterpret_cast<XMultiServiceFactory *>(pServiceManager),
                implname1,
                test21_createInstance,
                Sequence<OUString>( &serviceName1, 1),
                &globalModuleCount.modCnt
                ));
        
            if (xFactory.is())
            {
                xFactory->acquire();
                pRet = xFactory.get();
            }
        }
        else if( implname2.equals( OUString::createFromAscii(pImplName)))
        {
            Reference<XMultiServiceFactory> mgr= reinterpret_cast<XMultiServiceFactory *>(pServiceManager);
            Reference<XSingleServiceFactory> xFactory( createSingleFactory(
                reinterpret_cast<XMultiServiceFactory *>(pServiceManager),
                implname2,
                test22_createInstance,
                Sequence<OUString>( &serviceName2, 1),
                &globalModuleCount.modCnt
                ));
        
            if (xFactory.is())
            {
                xFactory->acquire();
                pRet = xFactory.get();
            }
        }
        else if( implname3.equals( OUString::createFromAscii(pImplName)))
        {
            Reference<XMultiServiceFactory> mgr= reinterpret_cast<XMultiServiceFactory *>(pServiceManager);
            Reference<XSingleServiceFactory> xFactory( createSingleFactory(
                reinterpret_cast<XMultiServiceFactory *>(pServiceManager),
                implname3,
                test23_createInstance,
                Sequence<OUString>( &serviceName3, 1),
                &globalModuleCount.modCnt
                ));
        
            if (xFactory.is())
            {
                xFactory->acquire();
                pRet = xFactory.get();
            }
        }
    
        return pRet;
    }

    sal_Bool component_canUnload( TimeValue* libUnused)
    {
        return globalModuleCount.canUnload( &globalModuleCount, libUnused);
    }
}
