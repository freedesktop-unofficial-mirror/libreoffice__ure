/*************************************************************************
 *
 *  $RCSfile: javaloader.cxx,v $
 *
 *  $Revision: 1.16 $
 *
 *  last change: $Author: sb $ $Date: 2002-12-06 10:51:53 $
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


#include <cstdarg>
#include <osl/diagnose.h>
#include <osl/process.h>

#include <rtl/ustring>
#include <rtl/process.h>
#include <rtl/ustrbuf.hxx>

#include <uno/environment.h>
#include <uno/mapping.hxx>

#include <cppuhelper/servicefactory.hxx>

#ifdef LINUX
#undef minor
#undef major
#endif

#include <com/sun/star/java/XJavaVM.hpp>

#include <com/sun/star/lang/XMultiComponentFactory.hpp>

#include "jni.h"

#include <cppuhelper/factory.hxx>
#include <cppuhelper/implementationentry.hxx>

#include <cppuhelper/implbase2.hxx>

#include <com/sun/star/loader/XImplementationLoader.hpp>
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/registry/XRegistryKey.hpp>

#include "jvmaccess/virtualmachine.hxx"

namespace css = com::sun::star;

using namespace ::com::sun::star::java;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::loader;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::registry;

using namespace ::cppu;
using namespace ::rtl;
using namespace ::osl;

namespace stoc_javaloader {
    static Sequence< OUString > loader_getSupportedServiceNames()
    {
        static Sequence < OUString > *pNames = 0;
        if( ! pNames )
        {
            MutexGuard guard( Mutex::getGlobalMutex() );
            if( !pNames )
            {
                static Sequence< OUString > seqNames(2);
                seqNames.getArray()[0] = OUString(
                    RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.loader.Java") );
                seqNames.getArray()[1] = OUString(
                    RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.loader.Java2") );
                pNames = &seqNames;
            }
        }
        return *pNames;
    }
    
    static OUString loader_getImplementationName()
    {
        static OUString *pImplName = 0;
        if( ! pImplName )
        {
            MutexGuard guard( Mutex::getGlobalMutex() );
            if( ! pImplName )
            {
                static OUString implName(
                    RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.stoc.JavaComponentLoader" ) );
                pImplName = &implName;
            }
        }
        return *pImplName;
    }

    class JavaComponentLoader : public WeakImplHelper2<XImplementationLoader, XServiceInfo> {
        css::uno::Reference<XImplementationLoader> _javaLoader;

    public:
        JavaComponentLoader(const css::uno::Reference<XComponentContext> & xCtx) throw(RuntimeException);
        virtual ~JavaComponentLoader() throw();

    public:
        // XServiceInfo
        virtual OUString           SAL_CALL getImplementationName()                      throw(RuntimeException);
        virtual sal_Bool           SAL_CALL supportsService(const OUString& ServiceName) throw(RuntimeException);
        virtual Sequence<OUString> SAL_CALL getSupportedServiceNames()                   throw(RuntimeException);

        // XImplementationLoader
        virtual css::uno::Reference<XInterface> SAL_CALL activate(const OUString& implementationName, const OUString& implementationLoaderUrl, const OUString& locationUrl, const css::uno::Reference<XRegistryKey>& xKey) throw(CannotActivateFactoryException, RuntimeException);
        virtual sal_Bool SAL_CALL writeRegistryInfo(const css::uno::Reference<XRegistryKey>& xKey, const OUString& implementationLoaderUrl, const OUString& locationUrl) throw(CannotRegisterImplementationException, RuntimeException);
    };

    JavaComponentLoader::JavaComponentLoader(const css::uno::Reference<XComponentContext> & xCtx) throw(RuntimeException)	
    {
        sal_Int32 size = 0;
        uno_Environment * pJava_environment = NULL;
        uno_Environment * pUno_environment = NULL;
        typelib_InterfaceTypeDescription * pType_XImplementationLoader = 0;

        try {
            // get a java vm, where we can create a loader
            css::uno::Reference<XJavaVM> javaVM_xJavaVM(
                xCtx->getValueByName(
                    OUString(RTL_CONSTASCII_USTRINGPARAM(
                                 "/singletons/"
                                 "com.sun.star.java.theJavaVirtualMachine"))),
                UNO_QUERY_THROW);

            // Use the special protocol of XJavaVM.getJavaVM:  If the passed in
            // process ID has an extra 17th byte of value zero, the returned any
            // contains a pointer to a jvmaccess::VirtualMachine, instead of the
            // underlying JavaVM pointer:
            Sequence<sal_Int8> processID(17);
            rtl_getGlobalProcessId(reinterpret_cast<sal_uInt8 *>(processID.getArray()));
            processID[16] = 0;

            // We get a non-refcounted pointer to a jvmaccess::VirtualMachine
            // from the XJavaVM service (the pointer is guaranteed to be valid
            // as long as our reference to the XJavaVM service lasts), and
            // convert the non-refcounted pointer into a refcounted one
            // immediately:
            OSL_ENSURE(sizeof (sal_Int64)
                           >= sizeof (jvmaccess::VirtualMachine *),
                       "Pointer cannot be represented as sal_Int64");
            sal_Int64 nPointer = reinterpret_cast< sal_Int64 >(
                static_cast< jvmaccess::VirtualMachine * >(0));
            javaVM_xJavaVM->getJavaVM(processID) >>= nPointer;
            rtl::Reference< jvmaccess::VirtualMachine > xVirtualMachine(
                reinterpret_cast< jvmaccess::VirtualMachine * >(nPointer));
            if (!xVirtualMachine.is())
                throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM(
                   "javaloader error - JavaVirtualMachine service could not provide a VM")), css::uno::Reference<XInterface>());

            try
            {
                jvmaccess::VirtualMachine::AttachGuard aGuard(xVirtualMachine);
                JNIEnv * pJNIEnv = aGuard.getEnvironment();

                // instantiate the java JavaLoader
                jclass jcJavaLoader = pJNIEnv->FindClass("com/sun/star/comp/loader/JavaLoader");
                if(pJNIEnv->ExceptionOccurred())
                    throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM(
                        "javaloader error - could not find class com/sun/star/comp/loader/JavaLoader")), css::uno::Reference<XInterface>());
                jmethodID jmJavaLoader_init = pJNIEnv->GetMethodID(jcJavaLoader, "<init>", "()V");
                if(pJNIEnv->ExceptionOccurred())
                    throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM(
                        "javaloader error - instantiation of com.sun.star.comp.loader.JavaLoader failed")), css::uno::Reference<XInterface>());
                jobject joJavaLoader = pJNIEnv->NewObject(jcJavaLoader, jmJavaLoader_init);
                if(pJNIEnv->ExceptionOccurred())
                    throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM(
                        "javaloader error - instantiation of com.sun.star.comp.loader.JavaLoader failed")), css::uno::Reference<XInterface>());

                // map the java JavaLoader to this environment
                OUString sJava(RTL_CONSTASCII_USTRINGPARAM("java"));
                uno_getEnvironment(&pJava_environment, sJava.pData,
                                   xVirtualMachine.get());
                if(!pJava_environment)
                    throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM(
                        "javaloader error - no Java environment available")), css::uno::Reference<XInterface>());

                // why is there no convinient contructor?
                OUString sCppu_current_lb_name(RTL_CONSTASCII_USTRINGPARAM(CPPU_CURRENT_LANGUAGE_BINDING_NAME));
                uno_getEnvironment(&pUno_environment, sCppu_current_lb_name.pData, NULL);
                if(!pUno_environment)
                    throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM(
                        "javaloader error - no C++ environment available")), css::uno::Reference<XInterface>());

                Mapping java_curr(pJava_environment, pUno_environment);
                if(!java_curr.is())
                    throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM(
                        "javaloader error - no mapping from java to C++ ")), css::uno::Reference<XInterface>());

                // release java environment
                pJava_environment->release(pJava_environment);
                pJava_environment = NULL;

                // release uno environment
                pUno_environment->release(pUno_environment);
                pUno_environment = NULL;

                getCppuType((css::uno::Reference<XImplementationLoader> *) 0).getDescription((typelib_TypeDescription **) & pType_XImplementationLoader);
                if(!pType_XImplementationLoader)
                    throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM(
                        "javaloader error - no type information for XImplementationLoader")), css::uno::Reference<XInterface>());

                _javaLoader = css::uno::Reference<XImplementationLoader>(reinterpret_cast<XImplementationLoader *>(
                              java_curr.mapInterface(joJavaLoader, pType_XImplementationLoader)));
                pJNIEnv->DeleteLocalRef( joJavaLoader );
                if(!_javaLoader.is())
                    throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM(
                        "javaloader error - mapping of java XImplementationLoader to c++ failed")), css::uno::Reference<XInterface>());

                typelib_typedescription_release(reinterpret_cast<typelib_TypeDescription *>(pType_XImplementationLoader));
                pType_XImplementationLoader = NULL;
            }
            catch (jvmaccess::VirtualMachine::AttachGuard::CreationException &)
            {
                throw RuntimeException(
                    OUString(RTL_CONSTASCII_USTRINGPARAM(
                                 "jvmaccess::VirtualMachine::AttachGuard"
                                 "::CreationException")),
                    0);
            }

            // set the service manager at the javaloader
            css::uno::Reference<XInitialization> javaLoader_XInitialization(_javaLoader, UNO_QUERY);
            if(!javaLoader_XInitialization.is())
                throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM(
                    "javaloader error - initialization of java javaloader failed, no XInitialization")), css::uno::Reference<XInterface>());

            Any any;
            any <<= css::uno::Reference<XMultiComponentFactory>(
                xCtx->getServiceManager());

            javaLoader_XInitialization->initialize(Sequence<Any>(&any, 1));
        }
        catch(RuntimeException &) {
            if(pJava_environment)
                pJava_environment->release(pJava_environment);

            if(pUno_environment)
                pUno_environment->release(pUno_environment);

            if(pType_XImplementationLoader)
                typelib_typedescription_release(reinterpret_cast<typelib_TypeDescription *>(pType_XImplementationLoader));

            throw;
        }
        OSL_TRACE("javaloader.cxx: mapped javaloader - 0x%x", _javaLoader.get());
    }


    JavaComponentLoader::~JavaComponentLoader() throw() {
    }

    // XServiceInfo
    OUString SAL_CALL JavaComponentLoader::getImplementationName() throw(::com::sun::star::uno::RuntimeException)
    {
        return loader_getImplementationName();	
    }	

    sal_Bool SAL_CALL JavaComponentLoader::supportsService(const OUString & ServiceName) throw(::com::sun::star::uno::RuntimeException)	{
        sal_Bool bSupport = sal_False;

        Sequence<OUString> aSNL = getSupportedServiceNames();
        const OUString * pArray = aSNL.getArray();
        for(sal_Int32 i = 0; i < aSNL.getLength() && !bSupport; ++ i)
            bSupport = pArray[i] == ServiceName;

        return bSupport;
    }	

    Sequence<OUString> SAL_CALL JavaComponentLoader::getSupportedServiceNames() throw(::com::sun::star::uno::RuntimeException)
    {
        return loader_getSupportedServiceNames();
    }



    // XImplementationLoader
    sal_Bool SAL_CALL JavaComponentLoader::writeRegistryInfo(const css::uno::Reference<XRegistryKey> & xKey, const OUString & blabla, const OUString & rLibName)
        throw(CannotRegisterImplementationException, RuntimeException)
    {
        return _javaLoader->writeRegistryInfo(xKey, blabla, rLibName);
    }


    css::uno::Reference<XInterface> SAL_CALL JavaComponentLoader::activate(const OUString & rImplName, 
                                                                 const OUString & blabla, 
                                                                 const OUString & rLibName,
                                                                 const css::uno::Reference<XRegistryKey> & xKey)
        throw(CannotActivateFactoryException, RuntimeException)
    {
        return _javaLoader->activate(rImplName, blabla, rLibName, xKey);
    }

    static Mutex & getInitMutex()
    {
        static Mutex * pMutex = 0;
        if( ! pMutex )
        {
            MutexGuard guard( Mutex::getGlobalMutex() );
            if( ! pMutex )
            {
                static Mutex mutex;
                pMutex = &mutex;
            }
        }
        return *pMutex;
    }
    css::uno::Reference<XInterface> SAL_CALL JavaComponentLoader_CreateInstance(const css::uno::Reference<XComponentContext> & xCtx) throw(Exception)
    {
        css::uno::Reference<XInterface> xRet;

        try {
            MutexGuard guard( getInitMutex() );
            // The javaloader is never destroyed and there can be only one!
            // Note that the first context wins ....
            static css::uno::Reference< XInterface > *pStaticRef = 0;
            if( pStaticRef )
            {
                xRet = *pStaticRef;
            }
            else
            {
                xRet = *new JavaComponentLoader(xCtx);
                pStaticRef = new css::uno::Reference< XInterface > ( xRet );
            }
        }
        catch(RuntimeException & runtimeException) {
            OString message = OUStringToOString(runtimeException.Message, RTL_TEXTENCODING_ASCII_US);
            osl_trace("javaloader - could not init javaloader cause of %s", message.getStr());
        }

        return xRet;
    }
}


using namespace stoc_javaloader;

static struct ImplementationEntry g_entries[] =
{
    {
        JavaComponentLoader_CreateInstance, loader_getImplementationName,
        loader_getSupportedServiceNames, createSingleComponentFactory,
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
