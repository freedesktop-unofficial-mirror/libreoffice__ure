/*************************************************************************
 *
 *  $RCSfile: IThreadPool.java,v $
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

package com.sun.star.lib.uno.environments.remote;

/**
 * This interface is an abstraction of the various
 * threadpool implementations.
 * <p>
 * @version 	$Revision: 1.1.1.1 $ $ $Date: 2000-09-18 15:27:52 $
 * @author 	    Joerg Budischewski
 * @see         com.sun.star.lib.uno.environments.remote.ThreadPool
 * @since       UDK1.0
 */
public interface IThreadPool {
    /**
     * Retrieves the global threadId for the current thread.
     * <p>
     * @return the thread id
     */
    public ThreadID getThreadId();
    

    /**
     * Adds a jobQueue for the current thread to the threadpool.
     * Requests are now put into this queue.
     * <p>
     * @param  disposeId    the dipose id with which the thread can be interrupted while staying in the queue
     * @see                 #enter
     */
    public void addThread(Object disposeId) throws InterruptedException;

    /**
     * Removes the jobQueue for the current thread.
     */
    public void removeThread() throws InterruptedException;

    /**
     * Queues a job into the jobQueue of the thread belonging to the jobs threadId.
     * <p>
     * @param job       the job
     * @param disposeId the dispose id
     */
    public void putJob(Job job, Object disposeId) throws InterruptedException;

    /**
     * Lets the current thread enter the ThreadPool.
     * The thread then dispatches all jobs and leaves
     * the ThreadPool when it gets a reply job.
     * <p>
     */
    public Object enter() throws java.lang.Exception;

    /**
     * Interrupts all threads which have associated the dispose id.
     * <p>
     * @param disposeId    the dispose id
     */
    public void dispose(Object disposeId);


    public void stopDispose(Object disposeId);
}

