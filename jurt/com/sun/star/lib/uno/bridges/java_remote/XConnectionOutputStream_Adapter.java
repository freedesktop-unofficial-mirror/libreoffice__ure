/*************************************************************************
 *
 *  $RCSfile: XConnectionOutputStream_Adapter.java,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: hr $ $Date: 2000-09-18 15:27:52 $
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

package com.sun.star.lib.uno.bridges.java_remote;


import java.io.IOException;
import java.io.OutputStream;

import com.sun.star.connection.XConnection;


class XConnectionOutputStream_Adapter extends OutputStream {
    static private final boolean DEBUG = false;

    protected XConnection _xConnection;
    protected byte _bytes[] = new byte[1];

    XConnectionOutputStream_Adapter(XConnection xConnection) {
        if(DEBUG) System.err.println("#### " + this.getClass()  + " - instantiated ");

        _xConnection = xConnection;
    }

    public void write(int b) throws IOException {
        _bytes[0] = (byte)b;

        try {
            _xConnection.write(_bytes);
        }
        catch(com.sun.star.io.IOException ioException) {
            throw new IOException(ioException.toString());
        }

        if(DEBUG) System.err.println("#### " + this.getClass()  + " - one byte written:" +  _bytes[0]);
    }

    public void write(byte[] b, int off, int len) throws IOException {
        byte bytes[] = new byte[len];

        System.arraycopy(b, off, bytes, 0, len);

        try {
            _xConnection.write(bytes);
        }
        catch(com.sun.star.io.IOException ioException) {
            throw new IOException(ioException.toString());
        }
    }

    public void flush() throws IOException {
        try {
            _xConnection.flush();
        }
        catch(com.sun.star.io.IOException ioException) {
            throw new IOException(ioException.toString());
        }
    }
}

