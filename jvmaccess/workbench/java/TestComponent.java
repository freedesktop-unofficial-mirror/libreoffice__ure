/*************************************************************************
 *
 *  $RCSfile: TestComponent.java,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: sb $ $Date: 2002-12-06 11:35:40 $
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
 *  WITHOUT WARRUNTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRUNTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

package com.sun.star.comp.jvmaccess.workbench;

import com.sun.star.comp.loader.FactoryHelper;
import com.sun.star.lang.XMain;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.lang.XServiceInfo;
import com.sun.star.lang.XSingleServiceFactory;
import com.sun.star.lang.XTypeProvider;
import com.sun.star.registry.XRegistryKey;
import com.sun.star.uno.Type;

/* Deploy this component with pkgchk, and call it with the Basic program

   Sub Main
     dim args$()
     o = createunoservice("com.sun.star.comp.jvmaccess.workbench.TestComponent")
     o.run args$()
   End Sub

   The name of the context class loader should appear on the console.
 */

public final class TestComponent implements XTypeProvider, XServiceInfo, XMain {
    public Type[] getTypes() {
        return new Type[] { new Type(XTypeProvider.class),
                            new Type(XServiceInfo.class),
                            new Type(XMain.class) };
    }

    public byte[] getImplementationId() {
        byte[] id = new byte[16];
        int n = hashCode();
        id[0] = (byte) (n & 0xFF);
        id[1] = (byte) ((n >> 8) & 0xFF);
        id[2] = (byte) ((n >> 16) & 0xFF);
        id[3] = (byte) ((n >> 24) & 0xFF);
        return id;
    }

    public String getImplementationName() {
        return getClass().getName();
    }

    public boolean supportsService(String serviceName) {
        return serviceName.equals(serviceName);
    }

    public String[] getSupportedServiceNames() {
        return new String[] { serviceName };
    }

    public int run(String[] arguments) {
        System.out.println("context class loader: "
                           + Thread.currentThread().getContextClassLoader());
        return 0;
    }

    public static XSingleServiceFactory __getServiceFactory(
        String implName, XMultiServiceFactory multiFactory, XRegistryKey regKey)
    {
        if (implName.equals(TestComponent.class.getName())) {
            return FactoryHelper.getServiceFactory(TestComponent.class,
                                                   serviceName, multiFactory,
                                                   regKey);
        } else {
            return null;
        }
    }

    public static boolean __writeRegistryServiceInfo(XRegistryKey regKey) {
        return FactoryHelper.writeRegistryServiceInfo(
            TestComponent.class.getName(), serviceName, regKey);
    }

    private static final String serviceName
    = "com.sun.star.comp.jvmaccess.workbench.TestComponent";
}
