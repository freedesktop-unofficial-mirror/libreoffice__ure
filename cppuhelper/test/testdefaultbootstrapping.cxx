/*************************************************************************
 *
 *  $RCSfile: testdefaultbootstrapping.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: kr $ $Date: 2001-07-24 12:20:32 $
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

#include <cstdio>

#include <rtl/process.h>
#include <cppuhelper/bootstrap.hxx>

#include <com/sun/star/lang/XMultiServiceFactory.hpp>

using namespace ::cppu;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;
using namespace ::rtl;


#if (defined UNX) || (defined OS2)
int main( int argc, char * argv[] )
#else
int __cdecl main( int argc, char * argv[] )
#endif
{
    sal_Bool result = sal_True;

    try {
        Reference<XComponentContext> xComponentContext = defaultBootstrap_InitialComponentContext();

        Reference<XMultiServiceFactory> smgr(xComponentContext->getServiceManager(), UNO_QUERY);

        for(sal_uInt32 i = 0; i < rtl_getAppCommandArgCount(); ++ i) {
            OUString arg;

            rtl_getAppCommandArg(i, &arg.pData);

            Reference<XInterface> xInterface = smgr->createInstance(arg);
            OString tmp = OUStringToOString(arg, RTL_TEXTENCODING_ASCII_US);
#ifdef DEBUG
            fprintf(stderr, "got the %s service %p\n", tmp.getStr(), xInterface.get());
#endif

            result = result && (xInterface.get() != 0);
        }
    }
    catch(Exception & exception) {
        OString message = OUStringToOString(exception.Message, RTL_TEXTENCODING_ASCII_US);

        fprintf(stderr, "an exception occurred: %s\n", message.getStr());
    }

#ifdef DEBUG
    OSL_TRACE("---------------------------------- %i", result);
#endif

    return result;
}
