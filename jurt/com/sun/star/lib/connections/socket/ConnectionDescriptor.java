/*************************************************************************
 *
 *  $RCSfile: ConnectionDescriptor.java,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: sb $ $Date: 2002-10-07 13:17:19 $
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

package com.sun.star.lib.connections.socket;

/**
 * Helper class for <code>socketAcceptor</code> and
 * <code>socketConnector</code>.
 *
 * <p>FIXME:  Once those classes have been moved from <code>jurt</code> to
 * <code>javaunohelper</code>, they should use
 * <code>com.sun.star.lib.uno.helper.UnoUrl</code> either instead of this class
 * or underneath this class.</p>
 */
final class ConnectionDescriptor {
    public ConnectionDescriptor(String description)
        throws com.sun.star.lang.IllegalArgumentException {
        for (int i = description.indexOf(','); i >= 0;) {
            int j = description.indexOf(',', i + 1);
            int k = j < 0 ? description.length() : j;
            int l = description.indexOf('=', i + 1);
            if (l < 0 || l >= k) {
                throw new com.sun.star.lang.IllegalArgumentException(
                    "parameter lacks '='");
            }
            String key = description.substring(i + 1, l);
            String value = description.substring(l + 1, k);
            if (key.equalsIgnoreCase("host")) {
                host = value;
            } else if (key.equalsIgnoreCase("port")) {
                try {
                    port = Integer.valueOf(value).intValue();
                } catch (NumberFormatException e) {
                    throw new com.sun.star.lang.IllegalArgumentException(
                        e.toString());
                }
                if (port < 0 || port > 65535) {
                    throw new com.sun.star.lang.IllegalArgumentException(
                        "port parameter must have value between 0 and 65535,"
                        + " inclusive");
                }
            } else if (key.equalsIgnoreCase("backlog")) {
                try {
                    backlog = Integer.valueOf(value).intValue();
                } catch (NumberFormatException e) {
                    throw new com.sun.star.lang.IllegalArgumentException(
                        e.toString());
                }
            } else if (key.equalsIgnoreCase("tcpnodelay")) {
                if (value.equals("0")) {
                    tcpNoDelay = Boolean.FALSE;
                } else if (value.equals("1")) {
                    tcpNoDelay = Boolean.TRUE;
                } else {
                    throw new com.sun.star.lang.IllegalArgumentException(
                        "tcpnodelay parameter must have 0/1 value");
                }
            }
            i = j;
        }
    }

    public String getHost() {
        return host;
    }

    public int getPort() {
        return port;
    }

    public int getBacklog() {
        return backlog;
    }

    public Boolean getTcpNoDelay() {
        return tcpNoDelay;
    }

    private String host = null;
    private int port = 6001;
    private int backlog = 50;
    private Boolean tcpNoDelay = null;
}
