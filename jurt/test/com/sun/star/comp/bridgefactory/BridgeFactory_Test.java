/*************************************************************************
 *
 *  $RCSfile: BridgeFactory_Test.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: kr $ $Date: 2001-01-17 09:33:25 $
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

package com.sun.star.comp.bridgefactory;


import java.util.Vector;


import com.sun.star.bridge.BridgeExistsException;
import com.sun.star.bridge.XBridge;

import com.sun.star.connection.XConnection;

import com.sun.star.comp.connections.PipedConnection;

import com.sun.star.lang.XComponent;

import com.sun.star.uno.UnoRuntime;


public class BridgeFactory_Test {
    static public boolean test(Vector notpassed) throws Exception {
        System.err.print("Testing BridgeFactory...");

        PipedConnection rightSide = new PipedConnection(new Object[0]);
        PipedConnection leftSide = new PipedConnection(new Object[]{rightSide});

        BridgeFactory bridgeFactory = new BridgeFactory(); // create the needed bridgeFactory

        // create a bridge
        XBridge xBridge = bridgeFactory.createBridge("testbridge", "urp", (XConnection)leftSide, null);

        // test that we get the same bridge
        boolean passed = UnoRuntime.areSame(xBridge, bridgeFactory.getBridge("testbridge"));

        // test that we can not create another bridge with same name
        try {
            XBridge dummy = bridgeFactory.createBridge("testbridge", "urp", (XConnection)leftSide, null);

            passed = false;
        }
        catch(BridgeExistsException bridgeExistsException) {
        }


        // test getExistingBridges
        XBridge xBridges[] = bridgeFactory.getExistingBridges();
        passed = UnoRuntime.areSame(xBridge, xBridges[0]) && passed;

        // dispose the bridge
        XComponent xComponent = (XComponent)UnoRuntime.queryInterface(XComponent.class, xBridge);
        xComponent.dispose();


        // test that the bridge has been removed
        passed = (bridgeFactory.getBridge("testbridge") == null) && passed;



        rightSide = new PipedConnection(new Object[0]);
        leftSide = new PipedConnection(new Object[]{rightSide});


        // test that we really get a new bridge
        XBridge xBridge_new = bridgeFactory.createBridge("testbridge", "urp", (XConnection)leftSide, null);
        passed = !UnoRuntime.areSame(xBridge, xBridge_new) && passed;

        for(int i = 0; i <10000; ++ i) {
            Object x[] = new Object[100];
        }

        // test getExistingBridges
        xBridges = bridgeFactory.getExistingBridges();
        if(xBridges.length == 1)
            passed = UnoRuntime.areSame(xBridge_new, xBridges[0]) && passed;
        else
            passed = false;

        // dispose the new bridge
        XComponent xComponent_new = (XComponent)UnoRuntime.queryInterface(XComponent.class, xBridge_new);
        xComponent_new.dispose();

        

        System.err.println(" passed? " + passed);
        if(!passed && notpassed != null)
            notpassed.addElement("BridgeFactory - passed? " + passed);

        return passed;
    }

    static public void main(String argv[]) throws Exception {
        test(null);
    }
}
