/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: socketAcceptor.java,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-07 18:55:32 $
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

package com.sun.star.lib.connections.socket;

import com.sun.star.comp.loader.FactoryHelper;
import com.sun.star.connection.AlreadyAcceptingException;
import com.sun.star.connection.ConnectionSetupException;
import com.sun.star.connection.XAcceptor;
import com.sun.star.connection.XConnection;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.lang.XSingleServiceFactory;
import com.sun.star.registry.XRegistryKey;
import java.io.IOException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;

/**
 * A component that implements the <code>XAcceptor</code> interface.
 *
 * <p>The <code>socketAcceptor</code> is a specialized component that uses TCP
 * sockets for communication.  The <code>socketAcceptor</code> is generally used
 * by the <code>com.sun.star.connection.Acceptor</code> service.</p>
 *
 * @see com.sun.star.connections.XAcceptor
 * @see com.sun.star.connections.XConnection
 * @see com.sun.star.connections.XConnector
 * @see com.sun.star.loader.JavaLoader
 *
 * @since UDK 1.0
 */
public final class socketAcceptor implements XAcceptor {
    /**
     * The name of the service.
     *
     * <p>The <code>JavaLoader</code> acceses this through reflection.</p>
     *
     * @see com.sun.star.comp.loader.JavaLoader
     */
    public static final String __serviceName
    = "com.sun.star.connection.socketAcceptor";

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
        return implName.equals(socketAcceptor.class.getName())
            ? FactoryHelper.getServiceFactory(socketAcceptor.class,
                                              __serviceName, multiFactory,
                                              regKey)
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
        return FactoryHelper.writeRegistryServiceInfo(
            socketAcceptor.class.getName(), __serviceName, regKey);
    }

    /**
     * Accepts a connection request via the described socket.
     *
     * <p>This call blocks until a connection has been established.</p>
     *
     * <p>The connection description has the following format:
     * <code><var>type</var></code><!--
     *     -->*(<code><var>key</var>=<var>value</var></code>),
     * where <code><var>type</var></code> should be <code>socket</code>
     * (ignoring case).  Supported keys (ignoring case) currently are
     * <dl>
     * <dt><code>host</code>
     * <dd>The name or address of the accepting interface (defaults to
     *     <code>0</code>, meaning any interface).
     * <dt><code>port</code>
     * <dd>The TCP port number to accept on (defaults to <code>6001</code>).
     * <dt><code>backlog</code>
     * <dd>The maximum length of the acceptor's queue (defaults to
     *     <code>50</code>).
     * <dt><code>tcpnodelay</code>
     * <dd>A flag (<code>0</code>/<code>1</code>) enabling or disabling Nagle's
     *     algorithm on the resulting connection.
     * </dl></p>
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
        ServerSocket serv;
        synchronized (this) {
            if (server == null) {
                ConnectionDescriptor desc
                    = new ConnectionDescriptor(connectionDescription);
                String host = desc.getHost();
                if (host.equals("0")) {
                    host = null;
                }
                if (DEBUG) {
                    System.err.println("##### " + getClass().getName()
                                       + ".accept: creating ServerSocket "
                                       + desc.getPort() + ", "
                                       + desc.getBacklog() + ", " + host);
                }
                try {
                    server = new ServerSocket(desc.getPort(), desc.getBacklog(),
                                              host == null ? null
                                              : InetAddress.getByName(host));
                } catch (IOException e) {
                    throw new ConnectionSetupException(e.toString());
                }
                acceptingDescription = connectionDescription;
                tcpNoDelay = desc.getTcpNoDelay();
            } else if (!connectionDescription.equals(acceptingDescription)) {
                throw new AlreadyAcceptingException(acceptingDescription
                                                    + " vs. "
                                                    + connectionDescription);
            }
            serv = server;
        }
        Socket socket;
        try {
            socket = serv.accept();
            if (DEBUG) {
                System.err.println("##### " + getClass().getName()
                                   + ".accept: accepted " + socket);
            }
            if (tcpNoDelay != null) {
                socket.setTcpNoDelay(tcpNoDelay.booleanValue());
            }
            return new SocketConnection(acceptingDescription, socket);
        }
        catch(IOException e) {
            throw new ConnectionSetupException(e.toString());
        }
    }

    // see com.sun.star.connection.XAcceptor#stopAccepting
    public void stopAccepting() {
        ServerSocket serv;
        synchronized (this) {
            serv = server;
        }
        try {
            serv.close();
        }
        catch (IOException e) {
            throw new com.sun.star.uno.RuntimeException(e.toString());
        }
    }

    private static final boolean DEBUG = false;

    private ServerSocket server = null;
    private String acceptingDescription;
    private Boolean tcpNoDelay;
}
