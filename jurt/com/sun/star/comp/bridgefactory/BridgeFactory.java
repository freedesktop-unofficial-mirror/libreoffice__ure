/*************************************************************************
 *
 *  $RCSfile: BridgeFactory.java,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: vg $ $Date: 2003-10-09 10:09:28 $
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


import java.util.Enumeration;
import java.util.Vector;


import com.sun.star.lang.XEventListener;
import com.sun.star.lang.XComponent;

import com.sun.star.bridge.BridgeExistsException;
import com.sun.star.bridge.XBridge;
import com.sun.star.bridge.XBridgeFactory;
import com.sun.star.bridge.XInstanceProvider;

import com.sun.star.comp.loader.FactoryHelper;

import com.sun.star.connection.XConnection;

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.lang.XSingleServiceFactory;

import com.sun.star.registry.XRegistryKey;

import com.sun.star.uno.IBridge;
import com.sun.star.uno.UnoRuntime;


/**
 * The BridgeFactory class implements the <code>XBridgeFactory</code> Interface.
 * It wrapps the <code>UnoRuntime#getBridgeByName</code>method and delivers a 
 * XBridge component.
 * <p>
 * This component is only usable for remote bridges.
 * <p>
 * @version 	$Revision: 1.8 $ $ $Date: 2003-10-09 10:09:28 $
 * @author 	    Kay Ramme
 * @see         com.sun.star.uno.UnoRuntime
 * @since       UDK1.0
 */
public class BridgeFactory implements XBridgeFactory/*, XEventListener*/ {
    static private final boolean DEBUG = false;

    /**
     * The name of the service, the <code>JavaLoader</code> acceses this through reflection.
     */
    public final static String __serviceName = "com.sun.star.bridge.BridgeFactory";

    /**
     * Gives a factory for creating the service.
     * This method is called by the <code>JavaLoader</code>
     * <p>
     * @return  returns a <code>XSingleServiceFactory</code> for creating the component
     * @param   implName     the name of the implementation for which a service is desired
     * @param   multiFactory the service manager to be uses if needed
     * @param   regKey       the registryKey
     * @see                  com.sun.star.comp.loader.JavaLoader
     */
    public static XSingleServiceFactory __getServiceFactory(String implName, 
                                                          XMultiServiceFactory multiFactory, 
                                                          XRegistryKey regKey)
    {
        XSingleServiceFactory xSingleServiceFactory = null;

        if (implName.equals(BridgeFactory.class.getName()) )
            xSingleServiceFactory = FactoryHelper.getServiceFactory(BridgeFactory.class,
                                                                    multiFactory, 
                                                                    regKey);
        
        return xSingleServiceFactory;
    }
    
    /**
     * Writes the service information into the given registry key.
     * This method is called by the <code>JavaLoader</code>
     * <p>
     * @return  returns true if the operation succeeded
     * @param   regKey       the registryKey
     * @see                  com.sun.star.comp.loader.JavaLoader
     */
    public static boolean __writeRegistryServiceInfo(XRegistryKey regKey) {
        return FactoryHelper.writeRegistryServiceInfo(BridgeFactory.class.getName(), __serviceName, regKey);
    }

    /**
     * Creates a remote bridge and memorizes it under <code>sName</code>.
     * <p>
     * @return   the bridge
     * @param    sName                the name to memorize the bridge
     * @param    sProtocol            the protocol the bridge should use
     * @param    anInstanceProvider   the instance provider
     * @see                           com.sun.star.bridge.XBridgeFactory
     */
    public XBridge createBridge(String sName, String sProtocol, XConnection aConnection, XInstanceProvider anInstanceProvider) throws 
        BridgeExistsException, 
        com.sun.star.lang.IllegalArgumentException, 
        com.sun.star.uno.RuntimeException
    {
        if(sName == null || sName.length() == 0)
            sName = sProtocol + ":" + aConnection.getDescription();

        // do not create a new bridge, if one already exists
        IBridge iBridges[] = UnoRuntime.getBridges();
        for(int i = 0; i < iBridges.length; ++ i) {
            XBridge xBridge = (XBridge)UnoRuntime.queryInterface(XBridge.class, iBridges[i]);

            if(xBridge != null) {
                if(xBridge.getName().equals(sName))
                    throw new BridgeExistsException(sName + " already exists");
            }
        }

        XBridge xBridge = null;

        try {
            IBridge iBridge = UnoRuntime.getBridgeByName("java", sName, "remote", sName, new Object[]{sProtocol, aConnection, anInstanceProvider, sName});

            xBridge = (XBridge)UnoRuntime.queryInterface(XBridge.class, iBridge);
        }
        catch(Exception exception) {
            throw new com.sun.star.lang.IllegalArgumentException(exception.getMessage());
        }

        if(DEBUG) System.err.println("##### " + getClass().getName() + ".createBridge:" + sName + " " + sProtocol + " " + aConnection + " "  + anInstanceProvider + " " + xBridge);

        return xBridge;
    }

    /**
     * Gets a remote bridge which must already exist.
     * <p>
     * @return   the bridge
     * @param    sName                the name of the bridge
     * @see                           com.sun.star.bridge.XBridgeFactory
     */
    public XBridge getBridge(String sName) throws com.sun.star.uno.RuntimeException {
        XBridge xBridge = null;

        IBridge iBridges[] = UnoRuntime.getBridges();
        for(int i = 0; i < iBridges.length; ++ i) {
            xBridge = (XBridge)UnoRuntime.queryInterface(XBridge.class, iBridges[i]);

            if(xBridge != null) {
                if(xBridge.getName().equals(sName))
                    break;
                    
                else
                    xBridge = null;
            }
        }

        
        if(DEBUG) System.err.println("##### " + getClass().getName() + ".getBridge:" + sName + " " + xBridge);

        return xBridge;
    }

    /**
     * Gives all created bridges
     * <p>
     * @return   the bridges
     * @see                           com.sun.star.bridge.XBridgeFactory
     */
    public synchronized XBridge[] getExistingBridges() throws com.sun.star.uno.RuntimeException {
        Vector vector = new Vector();

        IBridge iBridges[] = UnoRuntime.getBridges();
        for(int i = 0; i < iBridges.length; ++ i) {
            XBridge xBridge = (XBridge)UnoRuntime.queryInterface(XBridge.class, iBridges[i]);

            if(xBridge != null)
                vector.addElement(xBridge);
        }

        XBridge xBridges[]= new XBridge[vector.size()];
        for(int i = 0; i < vector.size(); ++ i)
            xBridges[i] = (XBridge)vector.elementAt(i);

        return xBridges;
    }
}

