/*************************************************************************
 *
 *  $RCSfile: DumpTypeDescription.java,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: kr $ $Date: 2001-05-08 09:54:01 $
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

package com.sun.star.tools.uno;


import com.sun.star.comp.helper.RegistryServiceFactory;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.registry.XImplementationRegistration;


import com.sun.star.lib.uno.typedesc.TypeDescription;

import com.sun.star.uno.IFieldDescription;
import com.sun.star.uno.IMethodDescription;
import com.sun.star.uno.ITypeDescription;
import com.sun.star.uno.Type;


public class DumpTypeDescription {
    static public void dumpTypeDescription(String typeName) throws Exception {
        ITypeDescription iTypeDescription = TypeDescription.getTypeDescription(typeName);

        System.err.println("TypeName:" + iTypeDescription.getTypeName());
        System.err.println("ArrayTypeName:" + iTypeDescription.getArrayTypeName());
        System.err.println("SuperType:" + iTypeDescription.getSuperType());
        System.err.println("TypeClass:" + iTypeDescription.getTypeClass());
        System.err.println("ComponentType:" + iTypeDescription.getComponentType());
        System.err.println("Class:" + iTypeDescription.getZClass());
        
        System.err.println("Methods:");
        IMethodDescription iMethodDescriptions[] = iTypeDescription.getMethodDescriptions();
        if(iMethodDescriptions != null)
            for(int i = 0; i < iMethodDescriptions.length; ++ i) {
                System.err.print("Name: " + iMethodDescriptions[i].getName());
                System.err.print(" index: " + iMethodDescriptions[i].getIndex());
                System.err.print(" isOneyWay: " + iMethodDescriptions[i].isOneway());
//  				System.err.print(" isConst: " + iMethodDescriptions[i].isConst());
                System.err.print(" isUnsigned: " + iMethodDescriptions[i].isUnsigned());
                System.err.print(" isAny: " + iMethodDescriptions[i].isAny());
                System.err.println("\tisInterface: " + iMethodDescriptions[i].isInterface());

                System.err.print("\tgetInSignature: ");
                ITypeDescription in_sig[] = iMethodDescriptions[i].getInSignature();
                for(int j = 0; j < in_sig.length; ++ j)
                    System.err.print("\t\t" + in_sig[j]);
                System.err.println();

                System.err.print("\tgetOutSignature: ");
                ITypeDescription out_sig[] = iMethodDescriptions[i].getOutSignature();
                for(int j = 0; j < out_sig.length; ++ j)
                    System.err.print("\t\t" + out_sig[j]);
                System.err.println();

                System.err.println("\tgetReturnSig: " + iMethodDescriptions[i].getReturnSignature());
                System.err.println("\tgetMethod:" + iMethodDescriptions[i].getMethod());
            }
        System.err.println();

        System.err.println("Members:");
        IFieldDescription iFieldDescriptions[] = iTypeDescription.getFieldDescriptions();
        if(iFieldDescriptions != null)
            for(int i = 0; i < iFieldDescriptions.length; ++ i) {
                System.err.print("\tMember: " + iFieldDescriptions[i].getName());
//  				System.err.print(" isConst: " + iMethodDescriptions[i].isConst());
                System.err.print(" isUnsigned: " + iFieldDescriptions[i].isUnsigned());
                System.err.print(" isAny: " + iFieldDescriptions[i].isAny());
                System.err.print("\tisInterface: " + iFieldDescriptions[i].isInterface());
                System.err.println("\tclass: " + iFieldDescriptions[i].getTypeDescription());

            }
    }

    static public void main(String args[]) throws Exception {
        if(args.length == 0)
            System.err.println("usage: [uno <type name>]|[java <class name>]*");

        else {
            int i = 0;
            while(i < args.length) {
                Type type = null;

                if(args[i].equals("uno"))
                    type = new Type(args[i + 1]);

                else
                    type = new Type(Class.forName(args[i + 1]));

                i += 2;


                dumpTypeDescription(type.getTypeName());
            }
        }
    }
}


