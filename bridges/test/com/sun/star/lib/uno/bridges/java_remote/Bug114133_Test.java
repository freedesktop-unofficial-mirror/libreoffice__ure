/*************************************************************************
 *
 *  $RCSfile: Bug114133_Test.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2004-06-04 03:03:16 $
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
 *  Copyright: 2002 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

package com.sun.star.lib.uno.bridges.javaremote;

import com.sun.star.bridge.XBridge;
import com.sun.star.bridge.XInstanceProvider;
import com.sun.star.lib.TestBed;
import com.sun.star.uno.XInterface;
import complexlib.ComplexTestCase;
import util.WaitUnreachable;

/**
 * Test case for bug #114133#.
 *
 * <p>Bug #114133# "Java UNO: UnoRuntime.getBride and disposed bridges."  The
 * client calls UnoRuntime.getBridge to get a bridge to the server, uses the
 * bridge, waits until it terminates itself (when all bridged objects have been
 * garbage-collected), then calls UnoRuntime.getBridge again.  This must return
 * a fresh, unterminated bridge.</p>
 */
public final class Bug114133_Test extends ComplexTestCase {
    public String[] getTestMethodNames() {
        return new String[] { "test" };
    }

    public void test() throws Exception {
        assure(
            "test",
            new TestBed().execute(new Provider(), false, Client.class, 0));
    }

    public static final class Client extends TestBed.Client {
        public static void main(String[] args) {
            new Client().execute();
        }

        protected boolean run(XBridge bridge) throws Throwable {
            new WaitUnreachable(bridge.getInstance("Test")).waitUnreachable();
            bridge = getBridge();
            new WaitUnreachable(bridge.getInstance("Test")).waitUnreachable();
            return true;
        }
    }

    private static final class Provider implements XInstanceProvider {
        public Object getInstance(String instanceName) {
            return new XInterface() {};
        }
    }
}
