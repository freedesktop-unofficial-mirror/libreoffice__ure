/*************************************************************************
 *
 *  $RCSfile: IProtocol.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: kr $ $Date: 2001-01-16 18:01:27 $
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

package com.sun.star.lib.uno.environments.remote;


import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import com.sun.star.uno.IBridge;



import com.sun.star.lib.uno.typedesc.TypeDescription;


/**
 * This interface provides an abstraction for protocols
 * for remote bridges.
 * <p>
 * @version 	$Revision: 1.3 $ $ $Date: 2001-01-16 18:01:27 $
 * @author 	    Kay Ramme
 * @see         com.sun.star.lib.uno.environments.remote.IMessage
 * @see         com.sun.star.lib.uno.environments.remote.Job
 * @since       UDK1.0
 */
public interface IProtocol {
    /**
     * Gets the name of the protocol.
     * <p>
     * @result  the name of the protocol
     */
    String getName();

    /**
     * Tells the protocol to ignore the next <code>closeConnection</code>
     * meta request.
     * <p>
     */
    void ignore_next_closeConnection();

    /**
     * Tells the protocol to send a <code>closeConnection</code>
     * meta request.
     * <p>
     * @param outputStream   the output stream
     */
    void send_closeConnection(OutputStream outputStream) throws IOException;


    /**
     * Reads a job from the given stream.
     * <p>
     * @return  thread read job.
     * @see     com.sun.star.lib.uno.environments.remote.Job
     */
    IMessage readMessage(InputStream inputStream) throws Exception;


    public void writeRequest(String oid, 
                             TypeDescription zInterface, 
                             String operation, 
                             ThreadID threadId, 
                             Object params[], 
                             Boolean synchron[],
                             Boolean mustReply[]) throws Exception;

    public void writeReply(boolean exception, ThreadID threadId, Object result) throws Exception;

    public void flush(java.io.DataOutput  dataOutput) throws Exception;


    /**
     * for testing and debugging
     */

    int getRequestsSendCount();

    int getRequestsReceivedCount();

    IMarshal createMarshal();
    IUnmarshal createUnmarshal(byte bytes[]);
}



