/*************************************************************************
 *
 *  $RCSfile: javavm.cxx,v $
 *
 *  $Revision: 1.21 $
 *
 *  last change: $Author: jl $ $Date: 2001-10-31 16:03:47 $
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

#ifdef UNX
#include <signal.h>
#endif

#include <time.h>


#include <osl/diagnose.h>
#include <osl/file.hxx>

#include <rtl/process.h>

#ifndef _OSL_MODULE_HXX_
#include <osl/module.hxx>
#endif

#ifndef _OSL_CONDITN_HXX_
#include <osl/conditn.hxx>
#endif

#ifndef _THREAD_HXX_
#include <osl/thread.hxx>
#endif

#include <uno/environment.h>

#include <cppuhelper/factory.hxx>
#include <cppuhelper/implementationentry.hxx>
#include <cppuhelper/implbase4.hxx>
#include <com/sun/star/uno/Sequence.hxx>

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/java/XJavaVM.hpp>
#include <com/sun/star/java/XJavaThreadRegister_11.hpp>

#include <com/sun/star/registry/XRegistryKey.hpp>
#include <com/sun/star/registry/XSimpleRegistry.hpp>
#include <com/sun/star/registry/InvalidRegistryException.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/container/XContainerListener.hpp>
#include <com/sun/star/container/XContainer.hpp>
#include <com/sun/star/container/XHierarchicalNameAccess.hpp>
#include <bridges/java/jvmcontext.hxx>

#include "jvmargs.hxx"

// Properties of the javavm can be put
// as a komma separated list in this
// environment variable
#define PROPERTIES_ENV "OO_JAVA_PROPERTIES"


#ifdef UNIX
#define INI_FILE "javarc"
#ifdef MACOSX
#define DEF_JAVALIB "JavaVM.framework"
#else
#define DEF_JAVALIB "libjvm.so"
#endif

#define TIMEZONE "MEZ"


#else
#define	INI_FILE "java.ini"
#define DEF_JAVALIB "javai.dll"

#define TIMEZONE "MET"


#endif 


using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::java;
using namespace com::sun::star::registry;
using namespace com::sun::star::beans;
using namespace com::sun::star::container;
using namespace rtl;
using namespace cppu;
using namespace osl;

namespace stoc_javavm {

    static Sequence< OUString > javavm_getSupportedServiceNames()
    {
        static Sequence < OUString > *pNames = 0;
        if( ! pNames )
        {
            MutexGuard guard( Mutex::getGlobalMutex() );
            if( !pNames )
            {
                static Sequence< OUString > seqNames(1);
                seqNames.getArray()[0] = OUString(
                    RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.java.JavaVirtualMachine") );
                pNames = &seqNames;
            }
        }
        return *pNames;
    }
    
    static OUString javavm_getImplementationName()
    {
        static OUString *pImplName = 0;
        if( ! pImplName )
        {
            MutexGuard guard( Mutex::getGlobalMutex() );
            if( ! pImplName )
            {
                static OUString implName(
                    RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.stoc.JavaVirtualMachine" ) );
                pImplName = &implName;
            }
        }
        return *pImplName;
    }
    
    static jint JNICALL vm_vfprintf( FILE *fp, const char *format, va_list args ) {
#ifdef DEBUG
        char buff[1024];
        
        vsprintf( buff, format, args );
        
        OSL_TRACE("%s", buff);
        return strlen(buff);
#else
        return 0;
#endif
    }

    static void JNICALL vm_exit(jint code) {
        OSL_TRACE("vm_exit: %d\n", code);
    }
    
    static void JNICALL vm_abort() {
        OSL_TRACE("vm_abort\n");
        abort();
    }
    
    typedef ::std::hash_map<sal_uInt32, sal_uInt32> UINT32_UINT32_HashMap;

    class JavaVirtualMachine_Impl;

    class OCreatorThread : public Thread {
        JavaVirtualMachine_Impl * _pJavaVirtualMachine_Impl;
        JavaVM                  * _pJVM;

        Condition _start_Condition;
        Condition _wait_Condition;

        JVM _jvm;
        RuntimeException _runtimeException;

    protected:
        virtual void SAL_CALL run() throw();

    public:
        OCreatorThread(JavaVirtualMachine_Impl * pJavaVirtualMachine_Impl) throw();
        JavaVM * createJavaVM(const JVM & jvm) throw (RuntimeException);
        void disposeJavaVM() throw();

    };

    class JavaVirtualMachine_Impl : public WeakImplHelper4< XJavaVM, XJavaThreadRegister_11,
                                    XServiceInfo, XContainerListener > {
        Mutex                           _Mutex;

        OCreatorThread                  _creatorThread;

        uno_Environment               * _pJava_environment;
        JavaVMContext                 * _pVMContext;

        Reference<XComponentContext>	    _xCtx;
        Reference<XMultiComponentFactory > _xSMgr;

        Reference<XInterface> _xConfigurationAccess;
        Module    _javaLib;

        void registerConfigChangesListener();
        
    public:
        OUString _error;

        JavaVirtualMachine_Impl(const Reference<XComponentContext> & xCtx) throw();
        ~JavaVirtualMachine_Impl() throw();


        // XJavaVM
        virtual Any      SAL_CALL getJavaVM(const Sequence<sal_Int8> & processID)	throw(RuntimeException);
        virtual sal_Bool SAL_CALL isVMStarted(void)                                 throw( RuntimeException);
        virtual sal_Bool SAL_CALL isVMEnabled(void)	                                throw( RuntimeException);

        // XJavaThreadRegister_11
        virtual sal_Bool SAL_CALL isThreadAttached(void) throw(RuntimeException);
        virtual void     SAL_CALL registerThread(void)   throw(RuntimeException);
        virtual void     SAL_CALL revokeThread(void)     throw(RuntimeException);

        // XServiceInfo
        virtual OUString           SAL_CALL getImplementationName()                      throw(RuntimeException);
        virtual sal_Bool           SAL_CALL supportsService(const OUString& ServiceName) throw(RuntimeException);
        virtual Sequence<OUString> SAL_CALL getSupportedServiceNames(void)               throw(RuntimeException);
    
        // XContainerListener
        virtual void SAL_CALL elementInserted( const ContainerEvent& Event ) throw (RuntimeException);
        virtual void SAL_CALL elementRemoved( const ContainerEvent& Event ) throw (RuntimeException);
        virtual void SAL_CALL elementReplaced( const ContainerEvent& Event ) throw (RuntimeException);

        // XEventListener
        virtual void SAL_CALL disposing( const EventObject& Source ) throw (RuntimeException);

        JavaVM *				createJavaVM(const JVM & jvm) throw(RuntimeException);
        void                    disposeJavaVM() throw();
    };

    OCreatorThread::OCreatorThread(JavaVirtualMachine_Impl * pJavaVirtualMachine_Impl) throw()
        : _pJVM(NULL)
    {
        _pJavaVirtualMachine_Impl = pJavaVirtualMachine_Impl;
    }

    void OCreatorThread::run() throw() {
        _start_Condition.wait();
        _start_Condition.reset();

        try {
            _pJVM = _pJavaVirtualMachine_Impl->createJavaVM(_jvm);
        }
        catch(RuntimeException & runtimeException) {
            _runtimeException = runtimeException;
        }
        
        _wait_Condition.set();

        if (_pJVM) {
#if defined(WNT) || defined(OS2)
            suspend();
            
#else
            _start_Condition.wait();
            _start_Condition.reset();
            
            _pJavaVirtualMachine_Impl->disposeJavaVM();
            
            _wait_Condition.set();
#endif
        }
    }

    JavaVM * OCreatorThread::createJavaVM(const JVM & jvm ) throw(RuntimeException) {
        _jvm = jvm;

        if (!_pJVM) {
            create();
        
            _start_Condition.set();

            _wait_Condition.wait();
            _wait_Condition.reset();

            if(!_pJVM)
                throw _runtimeException;
        }

        return _pJVM;
    }

    void OCreatorThread::disposeJavaVM() throw() {
        _start_Condition.set(); // start disposing vm

#ifdef UNX
        _wait_Condition.wait(); // wait until disposed
        _wait_Condition.reset();
#endif
    }



    // XServiceInfo
OUString SAL_CALL JavaVirtualMachine_Impl::getImplementationName() throw(RuntimeException)
{
    return javavm_getImplementationName();
}

    // XServiceInfo
sal_Bool SAL_CALL JavaVirtualMachine_Impl::supportsService(const OUString& ServiceName) throw(RuntimeException) {
    Sequence<OUString> aSNL = getSupportedServiceNames();
    const OUString * pArray = aSNL.getConstArray();
    
    for (sal_Int32 i = 0; i < aSNL.getLength(); ++ i)
        if (pArray[i] == ServiceName)
            return sal_True;
    
    return sal_False;
}

// XServiceInfo
Sequence<OUString> SAL_CALL JavaVirtualMachine_Impl::getSupportedServiceNames() throw(RuntimeException)
{
    return javavm_getSupportedServiceNames();
}
// XContainerListener 
void SAL_CALL JavaVirtualMachine_Impl::elementInserted( const ContainerEvent& Event )
    throw (RuntimeException)
{
}

// XContainerListener
void SAL_CALL JavaVirtualMachine_Impl::elementRemoved( const ContainerEvent& Event )
    throw (RuntimeException)
{
}

// XContainerListener
// If a user changes the setting, for example for proxy settings, then this function
// will be called from the configuration manager. Even if the .xml file does not contain
// an entry yet and that entry has to be inserted, this function will be called.
// We call java.lang.System.setProperty for the new values
void SAL_CALL JavaVirtualMachine_Impl::elementReplaced( const ContainerEvent& Event )
    throw (RuntimeException)
{
    MutexGuard aGuard( _Mutex);
    OUString sAccessor;
    Event.Accessor >>= sAccessor;
    OUString sPropertyName;
    OUString sPropertyValue;
    OUString sPropertyName2;
    if (sAccessor == OUString(RTL_CONSTASCII_USTRINGPARAM("ooInetFTPProxyName")))
    {
        sPropertyName= OUString(RTL_CONSTASCII_USTRINGPARAM("ftp.proxyHost"));
        Event.Element >>= sPropertyValue;
    }
    else if (sAccessor == OUString(RTL_CONSTASCII_USTRINGPARAM("ooInetFTPProxyPort")))
    {
        sPropertyName= OUString(RTL_CONSTASCII_USTRINGPARAM("ftp.proxyPort"));
        sPropertyValue= OUString::valueOf( *(sal_Int32*)Event.Element.getValue());
    }
    else if (sAccessor == OUString(RTL_CONSTASCII_USTRINGPARAM("ooInetHTTPProxyName")))
    {
        sPropertyName= OUString(RTL_CONSTASCII_USTRINGPARAM("http.proxyHost"));
        Event.Element >>= sPropertyValue;
    }
    else if (sAccessor == OUString(RTL_CONSTASCII_USTRINGPARAM("ooInetHTTPProxyPort")))
    {
        sPropertyName= OUString(RTL_CONSTASCII_USTRINGPARAM("http.proxyPort"));
        sPropertyValue= OUString::valueOf( *(sal_Int32*)Event.Element.getValue());
    }
    else if (sAccessor == OUString(RTL_CONSTASCII_USTRINGPARAM("ooInetNoProxy")))
    {
        sPropertyName= OUString(RTL_CONSTASCII_USTRINGPARAM("ftp.nonProxyHosts"));
        sPropertyName2= OUString(RTL_CONSTASCII_USTRINGPARAM("http.nonProxyHosts"));
        Event.Element >>= sPropertyValue;
        sPropertyValue= sPropertyValue.replace((sal_Unicode)';', (sal_Unicode)'|');
    }
    else if (sAccessor == OUString(RTL_CONSTASCII_USTRINGPARAM("ooInetSOCKSProxyName")))
    {
        sPropertyName= OUString(RTL_CONSTASCII_USTRINGPARAM("socksProxyHost"));
        Event.Element >>= sPropertyValue;
    }
    else if (sAccessor == OUString(RTL_CONSTASCII_USTRINGPARAM("ooInetSOCKSProxyPort")))
    {
        sPropertyName= OUString(RTL_CONSTASCII_USTRINGPARAM("socksProxyPort"));
        sPropertyValue= OUString::valueOf( *(sal_Int32*)Event.Element.getValue());			
    }
    else
        return;
    
    if (_pVMContext && _pVMContext->_pJavaVM)
    {
        JNIEnv* pJNIEnv= NULL;
        sal_Bool bThreadAttached= sal_False;
        jint ret= _pVMContext->_pJavaVM->AttachCurrentThread((void **)&pJNIEnv, (void*)NULL);
        OSL_ENSURE( !ret,"JavaVM could not attach current thread to VM");
        if ( ! _pVMContext->isThreadAttached())
        {
            bThreadAttached= sal_True;
        }
        
        // call java.lang.System.setProperty
        // String setProperty( String key, String value)
        jclass jcSystem= pJNIEnv->FindClass("java/lang/System");
        if(pJNIEnv->ExceptionOccurred()) throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("")), Reference<XInterface>());
        jmethodID jmSetProps= pJNIEnv->GetStaticMethodID( jcSystem, "setProperty","(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
        if(pJNIEnv->ExceptionOccurred()) throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("")), Reference<XInterface>());
        
        jstring jsPropName= pJNIEnv->NewString( sPropertyName.getStr(), sPropertyName.getLength());
        if(pJNIEnv->ExceptionOccurred()) throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("")), Reference<XInterface>());
        jstring jsPropValue= pJNIEnv->NewString( sPropertyValue.getStr(), sPropertyValue.getLength());
        if(pJNIEnv->ExceptionOccurred()) throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("")), Reference<XInterface>());
        jstring jsPrevValue= (jstring)pJNIEnv->CallStaticObjectMethod( jcSystem, jmSetProps, jsPropName, jsPropValue);
        if(pJNIEnv->ExceptionOccurred()) throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("")), Reference<XInterface>());
        
        // special calse for ftp.nonProxyHosts and http.nonProxyHosts. The office only
        // has a value for two java properties 
        if (sPropertyName2.getLength() > 0)
        {
            jstring jsPropName= pJNIEnv->NewString( sPropertyName2.getStr(), sPropertyName2.getLength());
            jstring jsPropValue= pJNIEnv->NewString( sPropertyValue.getStr(), sPropertyValue.getLength());
            if(pJNIEnv->ExceptionOccurred()) throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("")), Reference<XInterface>());
            jsPrevValue= (jstring)pJNIEnv->CallStaticObjectMethod( jcSystem, jmSetProps, jsPropName, jsPropValue);
            if(pJNIEnv->ExceptionOccurred()) throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("")), Reference<XInterface>());
        }
        if (bThreadAttached)
        {
            jint ret= _pVMContext->_pJavaVM->DetachCurrentThread();
            OSL_ENSURE( !ret,"JavaVM could not detach current thread to VM");
        }
    }
}

// XEventListenerListener
void SAL_CALL JavaVirtualMachine_Impl::disposing( const EventObject& Source )
    throw (RuntimeException)
{
    // In case the configuration manager wants to shut down
    if (_xConfigurationAccess.is() &&  Source.Source == _xConfigurationAccess)
    {
        Reference< XContainer > xContainer(_xConfigurationAccess, UNO_QUERY);
        if (xContainer.is())
            xContainer->removeContainerListener( static_cast< XContainerListener* >(this));
        _xConfigurationAccess= 0;
    }
    
    // If the service manager calls us then we are about to be shut down, therefore
    // unregister everywhere. Currently this service is only registered with the
    // configuration manager
    Reference< XInterface > xIntMgr( _xSMgr, UNO_QUERY);
    if (Source.Source == xIntMgr)
    {
        Reference< XContainer > xContainer(_xConfigurationAccess, UNO_QUERY);
        if (xContainer.is())
            xContainer->removeContainerListener( static_cast< XContainerListener* >(this));		
    }	
}

static void getDefaultLocaleFromConfig(JVM * pjvm,
                                       const Reference<XMultiComponentFactory> & xSMgr,
                                       const Reference<XComponentContext> &xCtx ) throw(Exception)
{
    Reference<XInterface> xConfRegistry = xSMgr->createInstanceWithContext(
        OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.configuration.ConfigurationRegistry")),
        xCtx );
    if(!xConfRegistry.is()) throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("javavm.cxx: couldn't get ConfigurationRegistry")), Reference<XInterface>());
    
    Reference<XSimpleRegistry> xConfRegistry_simple(xConfRegistry, UNO_QUERY);
    if(!xConfRegistry_simple.is()) throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("javavm.cxx: couldn't get ConfigurationRegistry")), Reference<XInterface>());
    
    xConfRegistry_simple->open(OUString(RTL_CONSTASCII_USTRINGPARAM("org.openoffice.Setup")), sal_True, sal_False);
    Reference<XRegistryKey> xRegistryRootKey = xConfRegistry_simple->getRootKey();
    
    // read locale
    Reference<XRegistryKey> locale = xRegistryRootKey->openKey(OUString(RTL_CONSTASCII_USTRINGPARAM("L10N/ooLocale")));
    if(locale.is() && locale->getStringValue().getLength()) {
        OUString language;
        OUString country;
        
        sal_Int32 index = locale->getStringValue().indexOf((sal_Unicode) '-');
        
        if(index >= 0) {
            language = locale->getStringValue().copy(0, index); 
            country = locale->getStringValue().copy(index + 1);
            
            if(language.getLength()) {
                OUString prop(RTL_CONSTASCII_USTRINGPARAM("user.language="));
                prop += language;
                
                pjvm->pushProp(prop);
            }
            
            if(country.getLength()) {
                OUString prop(RTL_CONSTASCII_USTRINGPARAM("user.region="));
                prop += country;
                
                pjvm->pushProp(prop);
            }
        }
    }
    
    xConfRegistry_simple->close();
}

static void setTimeZone(JVM * pjvm) throw() {
    /* A Bug in the Java function
    ** struct Hjava_util_Properties * java_lang_System_initProperties(
    ** struct Hjava_lang_System *this,
    ** struct Hjava_util_Properties *props);
    ** This function doesn't detect MEZ, MET or "W. Europe Standard Time"
    */
    struct tm *tmData;
    time_t clock = time(NULL);
    tzset();
    tmData = localtime(&clock);
#ifdef MACOSX
    char * p = tmData->tm_zone;
#else
    char * p = tzname[0];
#endif
    
    if (!strcmp(TIMEZONE, p))
        pjvm->pushProp(OUString::createFromAscii("user.timezone=ECT"));
}

static void getINetPropsFromConfig(JVM * pjvm,
                                   const Reference<XMultiComponentFactory> & xSMgr,
                                   const Reference<XComponentContext> &xCtx ) throw (Exception)
{
    Reference<XInterface> xConfRegistry = xSMgr->createInstanceWithContext(
            OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.configuration.ConfigurationRegistry")),
            xCtx );
    if(!xConfRegistry.is()) throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("javavm.cxx: couldn't get ConfigurationRegistry")), Reference<XInterface>());
    
    Reference<XSimpleRegistry> xConfRegistry_simple(xConfRegistry, UNO_QUERY);
    if(!xConfRegistry_simple.is()) throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("javavm.cxx: couldn't get ConfigurationRegistry")), Reference<XInterface>());
    
    xConfRegistry_simple->open(OUString(RTL_CONSTASCII_USTRINGPARAM("org.openoffice.Inet")), sal_True, sal_False);
    Reference<XRegistryKey> xRegistryRootKey = xConfRegistry_simple->getRootKey();
    
    // read ftp proxy name
    Reference<XRegistryKey> ftpProxy_name = xRegistryRootKey->openKey(OUString(RTL_CONSTASCII_USTRINGPARAM("Settings/ooInetFTPProxyName")));
    if(ftpProxy_name.is() && ftpProxy_name->getStringValue().getLength()) {
        OUString ftpHost = OUString(RTL_CONSTASCII_USTRINGPARAM("ftp.proxyHost="));
        ftpHost += ftpProxy_name->getStringValue();
        
        // read ftp proxy port
        Reference<XRegistryKey> ftpProxy_port = xRegistryRootKey->openKey(OUString(RTL_CONSTASCII_USTRINGPARAM("Settings/ooInetFTPProxyPort")));
        if(ftpProxy_port.is() && ftpProxy_port->getLongValue()) {
            OUString ftpPort = OUString(RTL_CONSTASCII_USTRINGPARAM("ftp.proxyPort="));
            ftpPort += OUString::valueOf(ftpProxy_port->getLongValue());
            
            pjvm->pushProp(ftpHost);
            pjvm->pushProp(ftpPort);
        }
    }
    
    // read http proxy name
    Reference<XRegistryKey> httpProxy_name = xRegistryRootKey->openKey(OUString(RTL_CONSTASCII_USTRINGPARAM("Settings/ooInetHTTPProxyName")));
    if(httpProxy_name.is() && httpProxy_name->getStringValue().getLength()) {
        OUString httpHost = OUString(RTL_CONSTASCII_USTRINGPARAM("http.proxyHost="));
        httpHost += httpProxy_name->getStringValue();
        
        // read http proxy port
        Reference<XRegistryKey> httpProxy_port = xRegistryRootKey->openKey(OUString(RTL_CONSTASCII_USTRINGPARAM("Settings/ooInetHTTPProxyPort")));
        if(httpProxy_port.is() && httpProxy_port->getLongValue()) {
            OUString httpPort = OUString(RTL_CONSTASCII_USTRINGPARAM("http.proxyPort="));
            httpPort += OUString::valueOf(httpProxy_port->getLongValue());
            
            pjvm->pushProp(httpHost);
            pjvm->pushProp(httpPort);
        }
    }
    
    // read  nonProxyHosts
    Reference<XRegistryKey> nonProxies_name = xRegistryRootKey->openKey(OUString(RTL_CONSTASCII_USTRINGPARAM("Settings/ooInetNoProxy")));
    if(nonProxies_name.is() && nonProxies_name->getStringValue().getLength()) {
        OUString httpNonProxyHosts = OUString(RTL_CONSTASCII_USTRINGPARAM("http.nonProxyHosts="));
        OUString ftpNonProxyHosts= OUString(RTL_CONSTASCII_USTRINGPARAM("ftp.nonProxyHosts="));
        OUString value= nonProxies_name->getStringValue();
        // replace the separator ";" by "|"
        value= value.replace((sal_Unicode)';', (sal_Unicode)'|');
        
        httpNonProxyHosts += value;
        ftpNonProxyHosts += value;
        
        pjvm->pushProp(httpNonProxyHosts);
        pjvm->pushProp(ftpNonProxyHosts);
    }
    
    // read socks settings
    Reference<XRegistryKey> socksProxy_name = xRegistryRootKey->openKey(OUString(RTL_CONSTASCII_USTRINGPARAM("Settings/ooInetSOCKSProxyName")));
    if (socksProxy_name.is() && httpProxy_name->getStringValue().getLength()) {
        OUString socksHost = OUString(RTL_CONSTASCII_USTRINGPARAM("socksProxyHost="));
        socksHost += socksProxy_name->getStringValue();
        
        // read http proxy port
        Reference<XRegistryKey> socksProxy_port = xRegistryRootKey->openKey(OUString(RTL_CONSTASCII_USTRINGPARAM("Settings/ooInetSOCKSProxyPort")));
        if (socksProxy_port.is() && socksProxy_port->getLongValue()) {
            OUString socksPort = OUString(RTL_CONSTASCII_USTRINGPARAM("socksProxyPort="));
            socksPort += OUString::valueOf(socksProxy_port->getLongValue());
            
            pjvm->pushProp(socksHost);
            pjvm->pushProp(socksPort);
        }
    }
    xConfRegistry_simple->close();
}

static void getJavaPropsFromConfig(JVM * pjvm,
                                   const Reference<XMultiComponentFactory> & xSMgr,
                                   const Reference<XComponentContext> &xCtx) throw(Exception)
{
    Reference<XInterface> xConfRegistry = xSMgr->createInstanceWithContext(
        OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.configuration.ConfigurationRegistry")),
        xCtx);
    if(!xConfRegistry.is()) throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("javavm.cxx: couldn't get ConfigurationRegistry")), Reference<XInterface>());
    
    Reference<XSimpleRegistry> xConfRegistry_simple(xConfRegistry, UNO_QUERY);
    if(!xConfRegistry_simple.is()) throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("javavm.cxx: couldn't get ConfigurationRegistry")), Reference<XInterface>());
    
    xConfRegistry_simple->open(OUString(RTL_CONSTASCII_USTRINGPARAM("org.openoffice.Setup")), sal_True, sal_False);
    Reference<XRegistryKey> xRegistryRootKey = xConfRegistry_simple->getRootKey();
    
    Reference<XRegistryKey> key_InstallPath = xRegistryRootKey->openKey(OUString(RTL_CONSTASCII_USTRINGPARAM("Office/ooSetupInstallPath")));
    if(!key_InstallPath.is()) throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("javavm.cxx: can not find key: Office/InstallPath in org.openoffice.UserProfile")), 
                                                     Reference<XInterface>());
    
    OUString rcPath = key_InstallPath->getStringValue();
    
    Reference<XInterface> xIniManager(xSMgr->createInstanceWithContext(
        OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.config.INIManager")),
        xCtx));
    if(!xIniManager.is()) throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("javavm.cxx: couldn't get: com.sun.star.config.INIManager")), Reference<XInterface>());
    
    Reference<XSimpleRegistry> xIniManager_simple(xIniManager, UNO_QUERY);
    if(!xIniManager_simple.is()) throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("javavm.cxx: couldn't get: com.sun.star.config.INIManager")), Reference<XInterface>());
    
    // normalize the path
    OUString urlrcPath;
    if( osl_File_E_None != File::getFileURLFromSystemPath( rcPath, urlrcPath ) )
    {
        urlrcPath = rcPath;
    }
    urlrcPath += OUString(RTL_CONSTASCII_USTRINGPARAM("/config/" INI_FILE));
    
    xIniManager_simple->open(urlrcPath, sal_True, sal_False);
    
    Reference<XRegistryKey> xJavaSection = xIniManager_simple->getRootKey()->openKey(OUString(RTL_CONSTASCII_USTRINGPARAM("Java")));
    if(!xJavaSection.is() || !xJavaSection->isValid())
        throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("javavm.cxx: can not find java section in " INI_FILE)), Reference<XInterface>());
    
    Sequence<OUString> javaProperties = xJavaSection->getKeyNames();
    OUString * pSectionEntry = javaProperties.getArray();
    sal_Int32 nCount         = javaProperties.getLength();
    
    for(sal_Int32 i=0; i < nCount; ++ i) 
    {
        //Reconstruct the whole lines of the java.ini
        Reference< XRegistryKey > key= xJavaSection->openKey(pSectionEntry[i]);
        if (key.is())
        {
            // there was a "=" in the line, hence key/value pair.
            OUString entryValue = key->getStringValue();	
            
            if(entryValue.getLength()) {
                pSectionEntry[i] += OUString(RTL_CONSTASCII_USTRINGPARAM("="));
                pSectionEntry[i] += entryValue;
            }
        }
        
        pjvm->pushProp(pSectionEntry[i]);
    }
    
    xIniManager_simple->close();
}

static void getJavaPropsFromSafetySettings(JVM * pjvm,
                                           const Reference<XMultiComponentFactory> & xSMgr,
                                           const Reference<XComponentContext> &xCtx) throw(Exception)
{
    Reference<XInterface> xConfRegistry = xSMgr->createInstanceWithContext(
        OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.configuration.ConfigurationRegistry")),
        xCtx);
    if(!xConfRegistry.is()) throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("javavm.cxx: couldn't get ConfigurationRegistry")), Reference<XInterface>());
    
    Reference<XSimpleRegistry> xConfRegistry_simple(xConfRegistry, UNO_QUERY);
    if(!xConfRegistry_simple.is()) throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("javavm.cxx: couldn't get ConfigurationRegistry")), Reference<XInterface>());
    
    xConfRegistry_simple->open(OUString(RTL_CONSTASCII_USTRINGPARAM("org.openoffice.Office.Java")), sal_True, sal_False);
    Reference<XRegistryKey> xRegistryRootKey = xConfRegistry_simple->getRootKey();
    
    if (xRegistryRootKey.is())
    {
        Reference<XRegistryKey> key_Enable = xRegistryRootKey->openKey(OUString(
            RTL_CONSTASCII_USTRINGPARAM("VirtualMachine/Enable")));
        if (key_Enable.is())
        {
            sal_Bool bEnableVal= key_Enable->getLongValue();
            pjvm->setEnabled( bEnableVal);
        }
        Reference<XRegistryKey> key_NetAccess = xRegistryRootKey->openKey(OUString(
            RTL_CONSTASCII_USTRINGPARAM("VirtualMachine/NetAccess")));
        if (key_NetAccess.is())
        {
            //????
            sal_Int32 nNetAccess= key_NetAccess->getLongValue();
        }
        Reference<XRegistryKey> key_UserClasspath = xRegistryRootKey->openKey(OUString(
            RTL_CONSTASCII_USTRINGPARAM("VirtualMachine/UserClassPath")));
        if (key_UserClasspath.is())
        {
            OUString sClassPath= key_UserClasspath->getStringValue();
            pjvm->setUserClasspath( sClassPath);
        }
    }
    xConfRegistry_simple->close();
}

static void getJavaPropsFromEnvironment(JVM * pjvm) throw() {
    // try some defaults for CLASSPATH and runtime lib
    const char * pClassPath = getenv("CLASSPATH");
    pjvm->setSystemClasspath(OUString::createFromAscii(pClassPath));
    
    pjvm->setRuntimeLib(OUString::createFromAscii(DEF_JAVALIB));
    pjvm->setEnabled(1);
    
    // See if properties have been set and parse them
    const char * pOOjavaProperties = getenv(PROPERTIES_ENV);
    if(pOOjavaProperties) {
        OUString properties(OUString::createFromAscii(pOOjavaProperties));
        
        sal_Int32 index;
        sal_Int32 lastIndex = 0;
        
        do {
            index = properties.indexOf((sal_Unicode)',', lastIndex);
            OUString token = (index == -1) ? properties.copy(lastIndex) : properties.copy(lastIndex, index - lastIndex);
            
            lastIndex = index + 1;
            
            pjvm->pushProp(token);
        }
        while(index > -1);
    }
}

static void initVMConfiguration(JVM * pjvm,
                                const Reference<XMultiComponentFactory> & xSMgr,
                                const Reference<XComponentContext > &xCtx) throw() {
    JVM jvm;
    try {
        getINetPropsFromConfig(&jvm, xSMgr, xCtx);
    }
    catch(Exception & exception) {
#ifdef DEBUG
        OString message = OUStringToOString(exception.Message, RTL_TEXTENCODING_ASCII_US);
        OSL_TRACE("javavm.cxx: can not get INetProps cause of >%s<", message.getStr());
#endif
    }
    
    try {
        getDefaultLocaleFromConfig(&jvm, xSMgr,xCtx);
    }
    catch(Exception & exception) {
#ifdef DEBUG
        OString message = OUStringToOString(exception.Message, RTL_TEXTENCODING_ASCII_US);
        OSL_TRACE("javavm.cxx: can not get locale cause of >%s<", message.getStr());
#endif
    }
    
    try {
        getJavaPropsFromConfig(&jvm, xSMgr,xCtx);
        
    }
    catch(Exception & exception) {
#ifdef DEBUG
        OString message = OUStringToOString(exception.Message, RTL_TEXTENCODING_ASCII_US);
        OSL_TRACE("javavm.cxx: couldn't use configuration cause of >%s<", message.getStr());
#endif
        
        getJavaPropsFromEnvironment(&jvm);
    }
    
    try {
        getJavaPropsFromSafetySettings(&jvm, xSMgr, xCtx);
    }
    catch(Exception & exception) {
#ifdef DEBUG
        OString message = OUStringToOString(exception.Message, RTL_TEXTENCODING_ASCII_US);
        OSL_TRACE("javavm.cxx: couldn't get safety settings because of >%s<", message.getStr());
#endif
    }
    *pjvm= jvm;
    setTimeZone(pjvm);
    
//  		pjvm->setPrint(vm_vfprintf);
//  		pjvm->setExit(vm_exit);
//  		pjvm->setAbort(vm_abort);
}

JavaVirtualMachine_Impl::JavaVirtualMachine_Impl(const Reference< XComponentContext > &xCtx) throw()
    : _pVMContext(NULL)
    ,  _creatorThread(this)
    ,  _pJava_environment(NULL)
    , _xSMgr( xCtx->getServiceManager() )
    , _xCtx( xCtx )
{
}

JavaVirtualMachine_Impl::~JavaVirtualMachine_Impl() throw() {
    if (_pVMContext)
        _creatorThread.disposeJavaVM();
    if (_xConfigurationAccess.is())
    {
        Reference< XContainer > xContainer(_xConfigurationAccess, UNO_QUERY);
        if (xContainer.is())
            xContainer->removeContainerListener( static_cast< XContainerListener* >(this));
    }
    if (_xSMgr.is())
    {
        Reference< XComponent > xComp(_xSMgr, UNO_QUERY);
        if (xComp.is())
            xComp->removeEventListener( static_cast< XEventListener* >(this));
    }
}


JavaVM * JavaVirtualMachine_Impl::createJavaVM(const JVM & jvm) throw(RuntimeException) {
    if(!_javaLib.load(jvm.getRuntimeLib())) {
        OUString message(RTL_CONSTASCII_USTRINGPARAM("JavaVirtualMachine_Impl::createJavaVM - cannot find java runtime: "));
        message += jvm.getRuntimeLib();
        
        throw RuntimeException(message, Reference<XInterface>());
    }
    
    JNI_InitArgs_Type * initArgs = (JNI_InitArgs_Type *)_javaLib.getSymbol(OUString::createFromAscii("JNI_GetDefaultJavaVMInitArgs"));
    JNI_CreateVM_Type * pCreateJavaVM = (JNI_CreateVM_Type *)_javaLib.getSymbol(OUString::createFromAscii("JNI_CreateJavaVM"));
    if (!initArgs || !pCreateJavaVM) {
        OUString message(RTL_CONSTASCII_USTRINGPARAM("JavaVirtualMachine_Impl::createJavaVM - cannot find symbols: JNI_GetDefaultJavaVMInitArgs or JNI_CreateJavaVM "));
        
        throw RuntimeException(message, Reference<XInterface>());
    }		
    
    JNIEnv * pJNIEnv = NULL;
    JavaVM * pJavaVM;
    
    // Try VM 1.1 
    JDK1_1InitArgs vm_args;
    vm_args.version= 0x00010001;
    jint ret= initArgs(&vm_args); 
    jvm.setArgs(&vm_args);
    
    jint err;
    err= pCreateJavaVM(&pJavaVM, &pJNIEnv, &vm_args);
    
    if( err != 0)
    {
        // Try VM 1.2 
        
        // The office sets a signal handler at startup. That causes a crash
        // with java 1.3 under Solaris. To make it work, we set back the 
        // handler
#ifdef UNX
        struct sigaction act;
        act.sa_handler=SIG_DFL;
        act.sa_flags= 0;
        sigaction( SIGSEGV, &act, NULL);
        sigaction( SIGPIPE, &act, NULL);
        sigaction( SIGBUS, &act, NULL);
        sigaction( SIGILL, &act, NULL);
        sigaction( SIGFPE, &act, NULL);
        
#endif			
        sal_uInt16 cprops= jvm.getProperties().size(); 			
        
        JavaVMInitArgs vm_args2;
        JavaVMOption * options= new JavaVMOption[cprops + 1];
        OString sClassPath= OString("-Djava.class.path=") + vm_args.classpath;
        options[0].optionString= (char*)sClassPath.getStr();
        options[0].extraInfo= NULL;
        
        OString * arProps= new OString[cprops];
        
        OString sPattern("-X");
        for( sal_uInt16 x= 0; x< cprops; x++)
        {				
            OString sOption(vm_args.properties[x]);
            
            if ( ! sOption.matchIgnoreAsciiCase(sPattern, 0))
                arProps[x]= OString("-D") + vm_args.properties[x];
            else
                arProps[x]= vm_args.properties[x];
            options[x+1].optionString= (char*)arProps[x].getStr();
            options[x+1].extraInfo= NULL;
        }
        vm_args2.version= 0x00010002;
        vm_args2.options= options;
        vm_args2.nOptions= cprops + 1;
        vm_args2.ignoreUnrecognized= JNI_TRUE;
        
        err= pCreateJavaVM(&pJavaVM, &pJNIEnv, &vm_args2);
        // Necessary to make debugging work. This thread will be suspended when this function
        // returns.
        if( err == 0)
            pJavaVM->DetachCurrentThread();
        
        delete [] options;
        delete [] arProps;
    }
    if(err) {
        OUString message(RTL_CONSTASCII_USTRINGPARAM("JavaVirtualMachine_Impl::createJavaVM - can not create vm, cause of err:"));
        message += OUString::valueOf((sal_Int32)err);
        
        throw RuntimeException(message, Reference<XInterface>());
    }
    
    return pJavaVM;
}

// XJavaVM
Any JavaVirtualMachine_Impl::getJavaVM(const Sequence<sal_Int8> & processId) throw (RuntimeException) {	
    MutexGuard guarg(_Mutex);
    
    Sequence<sal_Int8> localProcessID(16);
    rtl_getGlobalProcessId( (sal_uInt8*) localProcessID.getArray() );
    
    if (localProcessID == processId && !_pVMContext)
    {
        if(_error.getLength()) // do we have an error?
            throw RuntimeException(_error, Reference<XInterface>());

        uno_Environment ** ppEnviroments = NULL;
        sal_Int32 size = 0;
        OUString java(OUString::createFromAscii("java"));
        
        uno_getRegisteredEnvironments(&ppEnviroments, &size, (uno_memAlloc)malloc, java.pData);
        
        if(size) { // do we found an existing java environment?
            OSL_TRACE("javavm.cxx: found an existing environment");
            
            _pJava_environment = ppEnviroments[0];
            _pJava_environment->acquire(_pJava_environment);
            _pVMContext = (JavaVMContext *)_pJava_environment->pContext;
            
            for(sal_Int32 i = 0; i  < size; ++ i)
                ppEnviroments[i]->release(ppEnviroments[i]);
            
            free(ppEnviroments);
        }
        else
        {
            JVM jvm;
            JavaVM * pJavaVM;
            
            initVMConfiguration(&jvm, _xSMgr, _xCtx);
            
            if (jvm.isEnabled()) {
                    // create the java vm
                try {
                    pJavaVM = _creatorThread.createJavaVM(jvm);
                }
                catch(RuntimeException & runtimeException) {
                    // save the error message
                        _error = runtimeException.Message;
                        
                        throw;
                }
                
                    // create a context
                _pVMContext = new JavaVMContext(pJavaVM);
                
                    // register the java vm at the uno runtime
                uno_getEnvironment(&_pJava_environment, java.pData, _pVMContext);
                
                // listen for changes in the configuration, e.g. proxy settings.
                registerConfigChangesListener();
                }
        }
    }
    
    Any any;
    if(_pVMContext)
    {
        if(sizeof(_pVMContext->_pJavaVM) == sizeof(sal_Int32)) { // 32 bit system?
            sal_Int32 nP = (sal_Int32)_pVMContext->_pJavaVM;
            any <<= nP;
        }
        else if(sizeof(_pVMContext->_pJavaVM) == sizeof(sal_Int64)) { // 64 bit system?
            sal_Int64 nP = (sal_Int64)_pVMContext->_pJavaVM;
            any <<= nP;
        }
    }
    
    return any;
}

// XJavaVM
sal_Bool JavaVirtualMachine_Impl::isVMStarted(void) throw(RuntimeException) {
    return _pVMContext != NULL;
}

// XJavaVM
sal_Bool JavaVirtualMachine_Impl::isVMEnabled(void) throw(RuntimeException) {
    JVM jvm;
    
    initVMConfiguration(&jvm, _xSMgr, _xCtx);
    
    return jvm.isEnabled();
}

// XJavaThreadRegister_11
sal_Bool JavaVirtualMachine_Impl::isThreadAttached(void) throw (RuntimeException) {
    if(!_pVMContext)
        throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("JavaVirtualMachine_Impl::isThreadAttached - not vm context")), Reference<XInterface>());
    
    return _pVMContext->isThreadAttached();
}

// XJavaThreadRegister_11
void JavaVirtualMachine_Impl::registerThread(void) throw (RuntimeException) {
    if(!_pVMContext)
        throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("JavaVirtualMachine_Impl::registerThread - not vm context")), Reference<XInterface>());
    
    _pVMContext->registerThread();
}

// XJavaThreadRegister_11
void JavaVirtualMachine_Impl::revokeThread(void) throw (RuntimeException) {
    if(!_pVMContext)
        throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("JavaVirtualMachine_Impl::revokeThread - not vm context")), Reference<XInterface>());
    
    _pVMContext->revokeThread();
}


// JavaVirtualMachine_Impl_CreateInstance()
static Reference<XInterface> SAL_CALL JavaVirtualMachine_Impl_createInstance(const Reference<XComponentContext> & xCtx)
    throw (RuntimeException)
{
    Reference< XInterface > xRet;
    {
        MutexGuard guard( Mutex::getGlobalMutex() );
        // The javavm is never destroyed !
        static Reference< XInterface > *pStaticRef = 0;
        if( pStaticRef )
        {
            xRet = *pStaticRef;
        }
        else
            {
                xRet = *new JavaVirtualMachine_Impl( xCtx);
                pStaticRef = new Reference< XInterface> ( xRet );
            }
    }
    
        return xRet;
}

/*We listen to changes in the configuration. For example, the user changes the proxy
  settings in the options dialog (menu tools). Then we are notified of this change and
  if the java vm is already running we change the properties (System.lang.System.setProperties)
  through JNI.
  To receive notifications this class implements XContainerListener.
*/
void JavaVirtualMachine_Impl::registerConfigChangesListener()
{
    try
    {
        Reference< XMultiServiceFactory > xConfigProvider(
            _xSMgr->createInstanceWithContext( OUString( RTL_CONSTASCII_USTRINGPARAM(
                "com.sun.star.configuration.ConfigurationProvider")), _xCtx), UNO_QUERY);
        
        if (xConfigProvider.is())
        {
            // arguments for ConfigurationAccess
            Sequence< Any > aArguments(2);
            aArguments[0] <<= PropertyValue(
                OUString(RTL_CONSTASCII_USTRINGPARAM("nodepath")),
                0,
                makeAny(OUString(RTL_CONSTASCII_USTRINGPARAM("org.openoffice.Inet/Settings"))),
                PropertyState_DIRECT_VALUE);
            // depth: -1 means unlimited
            aArguments[1] <<= PropertyValue(
                OUString(RTL_CONSTASCII_USTRINGPARAM("depth")),
                0,
                makeAny( (sal_Int32)-1),
                PropertyState_DIRECT_VALUE);
            
            _xConfigurationAccess= xConfigProvider->createInstanceWithArguments(
                OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.configuration.ConfigurationAccess")),
                aArguments);
            Reference< XContainer > xContainer(_xConfigurationAccess, UNO_QUERY);
            
            if (xContainer.is())
                xContainer->addContainerListener( static_cast< XContainerListener* >(this));
            // The JavaVM service is registered as listener with the configuration service. That
            // service therefore keeps a reference of JavaVM. We need to unregister JavaVM with the
            // configuration service, otherwise the ref count of JavaVM won't drop to zero.		
            Reference< XComponent > xComp( _xSMgr, UNO_QUERY);
            if (xComp.is())
            {
                xComp->addEventListener( static_cast< XEventListener* >(this));
            }
        }	
    }catch( Exception & e)
    {
#ifdef DEBUG
        OString message = OUStringToOString(e.Message, RTL_TEXTENCODING_ASCII_US);
        OSL_TRACE("javavm.cxx: could not set up listener for Configuration because of >%s<", message.getStr());
#endif
    }
}

void JavaVirtualMachine_Impl::disposeJavaVM() throw() {
    if (_pVMContext){
//          pJavaVM->DestroyJavaVM();
//  			_pJavaVM = NULL;
    }
}
}

using namespace stoc_javavm;

static struct ImplementationEntry g_entries[] =
{
    {
        JavaVirtualMachine_Impl_createInstance, javavm_getImplementationName,
        javavm_getSupportedServiceNames, createSingleComponentFactory,
        0 , 0
    },
    { 0, 0, 0, 0, 0, 0 }
};

extern "C"
{
// NOTE: component_canUnload is not exported, as the library cannot be unloaded.
    
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
}
