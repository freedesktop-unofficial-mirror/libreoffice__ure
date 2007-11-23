/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: Bug111153_Test.java,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: ihi $ $Date: 2007-11-23 13:07:35 $
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

package com.sun.star.lib.uno.bridges.javaremote;

import com.sun.star.bridge.XInstanceProvider;
import com.sun.star.lib.TestBed;
import com.sun.star.lib.uno.typeinfo.MethodTypeInfo;
import com.sun.star.lib.uno.typeinfo.TypeInfo;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;
import com.sun.star.uno.XInterface;
import complexlib.ComplexTestCase;

/**
 * Test case for bug #111153#.
 *
 * <P>Bug #111153# "jni_uno bridge sometimes fails to map objects
 * correctly" describes that mapping a local object out with type XDerived and
 * then mapping it back in with type XBase produces a proxy, instead of
 * short-cutting to the local object.</P>
 */
public final class Bug111153_Test extends ComplexTestCase {
    public String[] getTestMethodNames() {
        return new String[] { "test" };
    }

    public void test() throws Exception {
        assure("test", new TestBed().execute(new Provider(), false,
                                             Client.class, 0));
    }

    public static final class Client extends TestBed.Client {
        public static void main(String[] args) {
            new Client().execute();
        }

        protected boolean run(XComponentContext context) throws Throwable {
            XTransport t = (XTransport) UnoRuntime.queryInterface(
                XTransport.class, getBridge(context).getInstance("Transport"));
            XDerived d = new XDerived() {};
            t.setDerived(d);
            return t.getBase() == d;
        }
    }

    private static final class Provider implements XInstanceProvider {
        public Object getInstance(String instanceName) {
            return new XTransport() {
                    public synchronized void setDerived(XDerived derived) {
                        this.derived = derived;
                    }

                    public synchronized XBase getBase() {
                        return this.derived;
                    }

                    private XDerived derived = null;
                };
        }
    }

    public interface XBase extends XInterface {
        TypeInfo[] UNOTYPEINFO = null;
    }

    public interface XDerived extends XBase {
        TypeInfo[] UNOTYPEINFO = null;
    }

    public interface XTransport extends XInterface {
        void setDerived(XDerived derived);

        XBase getBase();

        TypeInfo[] UNOTYPEINFO = { new MethodTypeInfo("setDerived", 0, 0),
                                   new MethodTypeInfo("getBase", 1, 0) };
    }
}
