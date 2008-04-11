/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: DumpType.java,v $
 * $Revision: 1.6 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

package com.sun.star.tools.uno;


import com.sun.star.comp.helper.RegistryServiceFactory;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.registry.XImplementationRegistration;

import com.sun.star.uno.Type;
/** A command-line tool. Prints information about com.sun.star.uno.Type to error stream.
 */
public class DumpType {
    static private final String[] __typeClassToTypeName = new String[]{
        "void",
        "char",
        "boolean",
        "byte",
        "short",
        "unsigned short",
        "long",
        "unsigned long",
        "hyper",
        "unsigned hyper", 
        "float",
        "double", 
        "string",
        "type",
        "any",
        "enum",
        "typedef",
        "struct",
        "union", 
        "exception",
        "sequence",
        "array",
        "interface",
        "service",
        "module",
        "interface_method",
        "interface_attribute",
        "unknown"
    };

    /** Prints information about argument type to the standard error stream.
        The information consists of type name, type description, class name,
        as well as the type class.
        @param type the Type object whoose information are printed.
     */
    static public void dumpType(Type type) throws Exception {
        System.err.println("uno type name:" + type.getTypeName());
        System.err.println("description:" + type.getTypeDescription());
        System.err.println("java class:" + type.getZClass());

        System.err.println("type class:" + __typeClassToTypeName[type.getTypeClass().getValue()]);
    }

    /** Executes this tool.
        The command line arguments consist of an identifier followed by a type name.
        Identifiers can either be uno or java. Based on the type name, a com.sun.star.uno.Type
        object is constructed whose information are then printed out to the error stream.
        <br>
        usage: [uno &lt;type name&gt;]|[java &lt;class name&gt]* <br>
        Examples: <br>
        java com.sun.star.uno.tools.DumpType uno com.sun.star.lang.XMultiServiceFactory
        java com.sun.star.uno.tools.DumpType java java.lang.Byte java java.lang.String
        @param args command line arguments
    */
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


                dumpType(type);
            }
        }
    }
}


