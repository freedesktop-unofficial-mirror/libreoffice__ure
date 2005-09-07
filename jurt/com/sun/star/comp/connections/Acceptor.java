/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: Acceptor.java,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-07 18:49:25 $
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

package com.sun.star.comp.connections;

import com.sun.star.comp.loader.FactoryHelper;
import com.sun.star.connection.AlreadyAcceptingException;
import com.sun.star.connection.ConnectionSetupException;
import com.sun.star.connection.XAcceptor;
import com.sun.star.connection.XConnection;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.lang.XSingleServiceFactory;
import com.sun.star.registry.XRegistryKey;

/**
 * A component that implements the <code>XAcceptor</code> interface.
 *
 * <p>The <code>Acceptor</code> is a general component, that uses less general
 * components (like <code>com.sun.star.connection.socketAcceptor</code>) to
 * implement its functionality.</p>
 *
 * @see com.sun.star.connections.XAcceptor
 * @see com.sun.star.connections.XConnection
 * @see com.sun.star.connections.XConnector
 * @see com.sun.star.loader.JavaLoader
 *
 * @since UDK 1.0
 */
public final class Acceptor implements XAcceptor {
    /**
     * The name of the service.
     *
     * <p>The <code>JavaLoader</code> acceses this through reflection.</p>
     *
     * @see com.sun.star.comp.loader.JavaLoader
     */
    public static final String __serviceName
    = "com.sun.star.connection.Acceptor";

    /**
     * Returns a factory for creating the service.
     *
     * <p>This method is called by the <code>JavaLoader</code>.</p>
     *
     * @param implName the name of the implementation for which a service is
     *     requested.
     * @param multiFactory the service manager to be used (if needed).
     * @param regKey the registry key.
     * @return an <code>XSingleServiceFactory</code> for creating the component.
     *
     * @see com.sun.star.comp.loader.JavaLoader
     */
    public static XSingleServiceFactory __getServiceFactory(
        String implName, XMultiServiceFactory multiFactory, XRegistryKey regKey)
    {
        return implName.equals(Acceptor.class.getName())
            ? FactoryHelper.getServiceFactory(Acceptor.class, __serviceName,
                                              multiFactory, regKey)
            : null;
    }

    /**
     * Writes the service information into the given registry key.
     *
     * <p>This method is called by the <code>JavaLoader</code>.</p>
     *
     * @param regKey the registry key.
     * @return <code>true</code> if the operation succeeded.
     *
     * @see com.sun.star.comp.loader.JavaLoader
     */
    public static boolean __writeRegistryServiceInfo(XRegistryKey regKey) {
        return FactoryHelper.writeRegistryServiceInfo(Acceptor.class.getName(),
                                                      __serviceName, regKey);
    }

    /**
     * Constructs a new <code>Acceptor</code> that uses the given service
     * factory to create a specific <code>XAcceptor</code>.
     *
     * @param serviceFactory the service factory to use.
     */
    public Acceptor(XMultiServiceFactory serviceFactory) {
        this.serviceFactory = serviceFactory;
    }

    /**
     * Accepts a connection request via the given connection type.
     *
     * <p>This call blocks until a connection has been established.</p>
     *
     * <p>The connection description has the following format:
     * <code><var>type</var></code><!--
     *     -->*(<code><var>key</var>=<var>value</var></code>).
     * The specific <code>XAcceptor</code> implementation is instantiated
     * through the service factory as
     * <code>com.sun.star.connection.<var>type</var>Acceptor</code> (with
     * <code><var>type</var></code> in lower case).</p>
     *
     * @param connectionDescription the description of the connection.
     * @return an <code>XConnection</code> to the client.
     *
     * @see com.sun.star.connections.XConnection
     * @see com.sun.star.connections.XConnector
     */
    public XConnection accept(String connectionDescription) throws
        AlreadyAcceptingException, ConnectionSetupException,
        com.sun.star.lang.IllegalArgumentException
    {
        if (DEBUG) {
            System.err.println("##### " + getClass().getName() + ".accept("
                               + connectionDescription + ")");
        }
        XAcceptor acc;
        synchronized (this) {
            if (acceptor == null) {
                acceptor = (XAcceptor) Implementation.getConnectionService(
                    serviceFactory, connectionDescription, XAcceptor.class,
                    "Acceptor");
                acceptingDescription = connectionDescription;
            } else if (!connectionDescription.equals(acceptingDescription)) {
                throw new AlreadyAcceptingException(acceptingDescription
                                                    + " vs. "
                                                    + connectionDescription);
            }
            acc = acceptor;
        }
        return acc.accept(connectionDescription);
    }

    // see com.sun.star.connection.XAcceptor#stopAccepting
    public void stopAccepting() {
        XAcceptor acc;
        synchronized (this) {
            acc = acceptor;
        }
        acc.stopAccepting();
    }

    private static final boolean DEBUG = false;

    private final XMultiServiceFactory serviceFactory;

    private XAcceptor acceptor = null;
    private String acceptingDescription;
}
