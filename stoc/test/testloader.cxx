/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: testloader.cxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-16 17:41:00 $
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
#include "precompiled_stoc.hxx"

#include <stdio.h>

#include <sal/main.h>
#ifndef _OSL_MODULE_H_
#include <osl/module.hxx>
#endif

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

#include <com/sun/star/loader/XImplementationLoader.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/lang/XSingleComponentFactory.hpp>

#include <cppuhelper/implbase1.hxx>
#ifndef _CPPUHELPER_FACTORY_HXX_
#include <cppuhelper/factory.hxx>
#endif

#if defined ( UNX ) || defined ( MAC )
#include <limits.h>
#define _MAX_PATH PATH_MAX
#endif

using namespace com::sun::star::uno;
using namespace com::sun::star::loader;
using namespace com::sun::star::lang;
using namespace osl;
using namespace rtl;
using namespace cppu;

#if OSL_DEBUG_LEVEL > 0
#define TEST_ENSHURE(c, m)   OSL_ENSURE(c, m)
#else
#define TEST_ENSHURE(c, m)   OSL_VERIFY(c)
#endif

class EmptyComponentContext : public WeakImplHelper1< XComponentContext >
{
public:
    virtual Any SAL_CALL getValueByName( const OUString& Name )
        throw (RuntimeException)
        {
            return Any();
        }
    virtual Reference< XMultiComponentFactory > SAL_CALL getServiceManager(  )
        throw (RuntimeException)
        {
            return Reference< XMultiComponentFactory > ();
        }

};


SAL_IMPLEMENT_MAIN()
{
    Reference<XInterface> xIFace;

    Module module;

    OUString dllName(
        RTL_CONSTASCII_USTRINGPARAM("shlibloader.uno" SAL_DLLEXTENSION) );
    
    if (module.load(dllName))
    {
        // try to get provider from module
        component_getFactoryFunc pCompFactoryFunc = (component_getFactoryFunc)
            module.getSymbol( OUString::createFromAscii(COMPONENT_GETFACTORY) );

        if (pCompFactoryFunc)
        {
            XSingleServiceFactory * pRet = (XSingleServiceFactory *)(*pCompFactoryFunc)(
                "com.sun.star.comp.stoc.DLLComponentLoader", 0, 0 );
            if (pRet)
            {
                xIFace = pRet;
                pRet->release();
            }
        }
    }

    TEST_ENSHURE( xIFace.is(), "testloader error1");

    Reference<XSingleComponentFactory> xFactory( Reference<XSingleComponentFactory>::query(xIFace) );

    TEST_ENSHURE( xFactory.is(), "testloader error2");

    Reference<XInterface> xLoader = xFactory->createInstanceWithContext( new EmptyComponentContext );

    TEST_ENSHURE( xLoader.is(), "testloader error3");

    Reference<XServiceInfo> xServInfo( Reference<XServiceInfo>::query(xLoader) );

    TEST_ENSHURE( xServInfo.is(), "testloader error4");

    TEST_ENSHURE( xServInfo->getImplementationName().equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.comp.stoc.DLLComponentLoader") ), "testloader error5");
    TEST_ENSHURE( xServInfo->supportsService(OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.loader.SharedLibrary")) ), "testloader error6");
    TEST_ENSHURE( xServInfo->getSupportedServiceNames().getLength() == 1, "testloader error7");

    xIFace.clear();
    xFactory.clear();
    xLoader.clear();
    xServInfo.clear();

    printf("Test Dll ComponentLoader, OK!\n");

    return(0);
}


