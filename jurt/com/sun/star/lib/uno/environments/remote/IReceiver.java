/*************************************************************************
 *
 *  $RCSfile: IReceiver.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: kr $ $Date: 2001-05-04 11:56:03 $
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

import com.sun.star.lib.uno.environments.java.IRequester;

/**
 * This interface is an abstraction for giving back
 * a reply for a request.
 * <p>
 * @version 	$Revision: 1.2 $ $ $Date: 2001-05-04 11:56:03 $
 * @author 	    Kay Ramme
 * @see         com.sun.star.uno.IQueryInterface
 */
public interface IReceiver {
    /**
     * Sends back a reply for the passed request id.
     * <p>
     * @param  threadId    the thread id of the request
     * @param  requestId   the request id
     * @param  status      the status of the request
     * @param  result      the result of the request
     * @param  resultClass the type of the result
     * @param  params      the out and in/out parameters passed with the request
     * @param  signature   the types of the out and in/out parameters
     */
    public void sendReply(boolean exception, ThreadID threadId, Object result);
}

