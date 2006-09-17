/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: vm.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 16:07:05 $
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
#include "precompiled_javaunohelper.hxx"

#include "sal/config.h"

#include "vm.hxx"

#include "com/sun/star/beans/NamedValue.hpp"
#include "com/sun/star/lang/XSingleComponentFactory.hpp"
#include "cppuhelper/compbase1.hxx"
#include "cppuhelper/component_context.hxx"
#include "jvmaccess/virtualmachine.hxx"
#include "jvmaccess/unovirtualmachine.hxx"
#include "osl/mutex.hxx"

namespace {

namespace css = ::com::sun::star;

struct MutexHolder
{
    ::osl::Mutex m_mutex;
};
typedef ::cppu::WeakComponentImplHelper1<
    css::lang::XSingleComponentFactory > t_impl;

class SingletonFactory : public MutexHolder, public t_impl
{
    ::rtl::Reference< ::jvmaccess::UnoVirtualMachine > m_vm_access;
    
protected:
    virtual void SAL_CALL disposing();
    
public:
    inline SingletonFactory( ::rtl::Reference< ::jvmaccess::UnoVirtualMachine > const & vm_access )
        : t_impl( m_mutex ),
          m_vm_access( vm_access )
        {}
    
    // XSingleComponentFactory impl
    virtual css::uno::Reference< css::uno::XInterface > SAL_CALL createInstanceWithContext(
        css::uno::Reference< css::uno::XComponentContext > const & xContext )
        throw (css::uno::Exception);
    virtual css::uno::Reference< css::uno::XInterface > SAL_CALL createInstanceWithArgumentsAndContext(
        css::uno::Sequence< css::uno::Any > const & args, css::uno::Reference< css::uno::XComponentContext > const & xContext )
        throw (css::uno::Exception);
};

void SingletonFactory::disposing()
{
    m_vm_access.clear();
}

css::uno::Reference< css::uno::XInterface > SingletonFactory::createInstanceWithContext(
    css::uno::Reference< css::uno::XComponentContext > const & xContext )
    throw (css::uno::Exception)
{
    sal_Int64 handle = reinterpret_cast< sal_Int64 >( m_vm_access.get() );
    css::uno::Any arg(
        css::uno::makeAny(
            css::beans::NamedValue(
                rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM( "UnoVirtualMachine" ) ),
                css::uno::makeAny( handle ) ) ) );
    return xContext->getServiceManager()->createInstanceWithArgumentsAndContext(
        ::rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM(
                "com.sun.star.java.JavaVirtualMachine")),
        css::uno::Sequence< css::uno::Any >( &arg, 1 ), xContext );
}

css::uno::Reference< css::uno::XInterface > SingletonFactory::createInstanceWithArgumentsAndContext(
    css::uno::Sequence< css::uno::Any > const & args, css::uno::Reference< css::uno::XComponentContext > const & xContext )
    throw (css::uno::Exception)
{
    return xContext->getServiceManager()->createInstanceWithArgumentsAndContext(
        ::rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM(
                "com.sun.star.java.JavaVirtualMachine")),
        args, xContext );
}

}

namespace javaunohelper {

::rtl::Reference< ::jvmaccess::UnoVirtualMachine > create_vm_access(
    JNIEnv * jni_env, jobject loader )
{
    JavaVM * vm;
    jni_env->GetJavaVM( &vm );
    try {
        return new ::jvmaccess::UnoVirtualMachine(
            new ::jvmaccess::VirtualMachine(
                vm, JNI_VERSION_1_2, false, jni_env ),
            loader );
    } catch ( ::jvmaccess::UnoVirtualMachine::CreationException & ) {
        throw css::uno::RuntimeException(
            ::rtl::OUString(
                RTL_CONSTASCII_USTRINGPARAM(
                    "jmvaccess::UnoVirtualMachine::CreationException"
                    " occurred" ) ),
            css::uno::Reference< css::uno::XInterface >() );
    }
}

css::uno::Reference< css::uno::XComponentContext > install_vm_singleton(
    css::uno::Reference< ::css::uno::XComponentContext > const & xContext,
    ::rtl::Reference< ::jvmaccess::UnoVirtualMachine > const & vm_access )
{
    css::uno::Reference< css::lang::XSingleComponentFactory > xFac( new SingletonFactory( vm_access ) );
    ::cppu::ContextEntry_Init entry(
        ::rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM(
                "/singletons/com.sun.star.java.theJavaVirtualMachine")),
        css::uno::makeAny( xFac ), true );
    return ::cppu::createComponentContext( &entry, 1, xContext );
}

}
