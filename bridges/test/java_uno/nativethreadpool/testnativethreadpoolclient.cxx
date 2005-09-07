/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: testnativethreadpoolclient.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-07 23:01:41 $
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

#include "test/javauno/nativethreadpool/XRelay.hpp"
#include "test/javauno/nativethreadpool/XSource.hpp"

#include "com/sun/star/bridge/UnoUrlResolver.hpp"
#include "com/sun/star/bridge/XUnoUrlResolver.hpp"
#include "com/sun/star/connection/ConnectionSetupException.hpp"
#include "com/sun/star/connection/NoConnectException.hpp"
#include "com/sun/star/lang/IllegalArgumentException.hpp"
#include "com/sun/star/lang/WrappedTargetRuntimeException.hpp"
#include "com/sun/star/lang/XMain.hpp"
#include "com/sun/star/lang/XMultiComponentFactory.hpp"
#include "com/sun/star/uno/Exception.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/RuntimeException.hpp"
#include "com/sun/star/uno/Sequence.hxx"
#include "com/sun/star/uno/XComponentContext.hpp"
#include "com/sun/star/uno/XInterface.hpp"
#include "cppuhelper/factory.hxx"
#include "cppuhelper/implbase2.hxx"
#include "cppuhelper/implementationentry.hxx"
#include "cppuhelper/weak.hxx"
#include "osl/thread.hxx"
#include "rtl/ustring.hxx"
#include "sal/types.h"
#include "uno/lbnames.h"

#include <iostream>

namespace css = com::sun::star;

namespace {

class Client: public cppu::WeakImplHelper2<
    css::lang::XMain, test::javauno::nativethreadpool::XSource >
{
public:
    explicit Client(
        css::uno::Reference< css::uno::XComponentContext > const & theContext):
        context(theContext) {}

private:
    virtual ~Client() {}

    virtual sal_Int32 SAL_CALL run(css::uno::Sequence< rtl::OUString > const &)
        throw (css::uno::RuntimeException);

    virtual sal_Int32 SAL_CALL get() throw (css::uno::RuntimeException);

    css::uno::Reference< css::uno::XComponentContext > context;
    osl::ThreadData data;
};

sal_Int32 Client::run(css::uno::Sequence< rtl::OUString > const &)
    throw (css::uno::RuntimeException)
{
    css::uno::Reference< css::lang::XMultiComponentFactory > factory(
        context->getServiceManager());
    if (!factory.is()) {
        throw new css::uno::RuntimeException(
            rtl::OUString::createFromAscii(
                "no component context service manager"),
            static_cast< cppu::OWeakObject * >(this));
    }
    css::uno::Reference< test::javauno::nativethreadpool::XRelay > relay;
    try {
        relay = css::uno::Reference< test::javauno::nativethreadpool::XRelay >(
            factory->createInstanceWithContext(
                rtl::OUString::createFromAscii(
                    "test.javauno.nativethreadpool.Relay"),
                context),
            css::uno::UNO_QUERY_THROW);
    } catch (css::uno::RuntimeException &) {
        throw;
    } catch (css::uno::Exception & e) {
        throw css::lang::WrappedTargetRuntimeException(
            rtl::OUString::createFromAscii(
                "creating test.javauno.nativethreadpool.Relay service"),
            static_cast< cppu::OWeakObject * >(this), css::uno::makeAny(e));
    }
    relay->start(this);
    if (!data.setData(reinterpret_cast< void * >(12345))) {
        throw new css::uno::RuntimeException(
            rtl::OUString::createFromAscii("osl::ThreadData::setData failed"),
            static_cast< cppu::OWeakObject * >(this));
    }
    css::uno::Reference< test::javauno::nativethreadpool::XSource > source;
    try {
        source
            = css::uno::Reference< test::javauno::nativethreadpool::XSource >(
                css::bridge::UnoUrlResolver::create(context)->resolve(
                    rtl::OUString::createFromAscii(
                        "uno:socket,host=localhost,port=3830;urp;test")),
                css::uno::UNO_QUERY_THROW);
    } catch (css::connection::NoConnectException & e) {
        throw css::lang::WrappedTargetRuntimeException(
            rtl::OUString::createFromAscii(
                "com.sun.star.uno.UnoUrlResolver.resolve"),
            static_cast< cppu::OWeakObject * >(this), css::uno::makeAny(e));
    } catch (css::connection::ConnectionSetupException & e) {
        throw css::lang::WrappedTargetRuntimeException(
            rtl::OUString::createFromAscii(
                "com.sun.star.uno.UnoUrlResolver.resolve"),
            static_cast< cppu::OWeakObject * >(this), css::uno::makeAny(e));
    } catch (css::lang::IllegalArgumentException & e) {
        throw css::lang::WrappedTargetRuntimeException(
            rtl::OUString::createFromAscii(
                "com.sun.star.uno.UnoUrlResolver.resolve"),
            static_cast< cppu::OWeakObject * >(this), css::uno::makeAny(e));
    }
    bool success = source->get() == 12345;
    std::cout << "success? " << (success ? "yes" : "no") << '\n';
    return success ? 0 : 1;
}

sal_Int32 Client::get() throw (css::uno::RuntimeException) {
    return reinterpret_cast< sal_Int32 >(data.getData());
}

css::uno::Reference< css::uno::XInterface > SAL_CALL create(
    css::uno::Reference< css::uno::XComponentContext > const & context)
    SAL_THROW((css::uno::Exception))
{
    return static_cast< cppu::OWeakObject * >(new Client(context));
}

rtl::OUString SAL_CALL getImplementationName() {
    return rtl::OUString::createFromAscii(
        "test.javauno.nativethreadpool.client");
}

css::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames() {
    return css::uno::Sequence< rtl::OUString >();
}

cppu::ImplementationEntry entries[] = {
    { &create, &getImplementationName, &getSupportedServiceNames,
      &cppu::createSingleComponentFactory, 0, 0 } };

}

extern "C" void * SAL_CALL component_getFactory(
    char const * implName, void * serviceManager, void * registryKey)
{
    return cppu::component_getFactoryHelper(
        implName, serviceManager, registryKey, entries);
}

extern "C" void SAL_CALL component_getImplementationEnvironment(
    char const ** envTypeName, uno_Environment **)
{
    *envTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}
