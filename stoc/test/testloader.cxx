/*************************************************************************
 *
 *  $RCSfile: testloader.cxx,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: hr $ $Date: 2000-09-18 15:29:36 $
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

#include <stdio.h>

#ifndef _VOS_MODULE_HXX_
#include <vos/module.hxx>
#endif
#ifndef _VOS_DIAGNOSE_HXX_
#include <vos/diagnose.hxx>
#endif


#include <com/sun/star/loader/XImplementationLoader.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>

#ifndef _CPPUHELPER_FACTORY_HXX_
#include <cppuhelper/factory.hxx>
#endif

#ifndef _VOS_DYNLOAD_HXX_
#include <vos/dynload.hxx>
#endif

#if defined ( UNX ) || defined ( MAC )
#include <limits.h>
#define _MAX_PATH PATH_MAX
#endif

using namespace com::sun::star::uno;
using namespace com::sun::star::loader;
using namespace com::sun::star::lang;
using namespace vos;
using namespace rtl;

#ifdef _DEBUG
#define TEST_ENSHURE(c, m)   VOS_ENSHURE(c, m)
#else
#define TEST_ENSHURE(c, m)   VOS_VERIFY(c)
#endif

#if (defined UNX) || (defined OS2)
int main( int argc, char * argv[] )
#else
int _cdecl main( int argc, char * argv[] )
#endif
{
    Reference<XInterface> xIFace;

    OModule* pModule = new OModule(OUString());

    OUString dllName;
    NAMESPACE_VOS(ORealDynamicLoader)::computeLibraryName(OUString::createFromAscii("cpld"), dllName);
        
    if (pModule->load(dllName))
    {
        // try to get provider from module
        component_getFactoryFunc pCompFactoryFunc = (component_getFactoryFunc)
            pModule->getSymbol( OUString::createFromAscii(COMPONENT_GETFACTORY) );
        
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

    Reference<XSingleServiceFactory> xFactory( Reference<XSingleServiceFactory>::query(xIFace) );
    
    TEST_ENSHURE( xFactory.is(), "testloader error2");

    Reference<XInterface> xLoader = xFactory->createInstance();	

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

    delete pModule;	
    
    printf("Test Dll ComponentLoader, OK!\n");

    return(0);
}


