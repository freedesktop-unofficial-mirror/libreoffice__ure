/*************************************************************************
 *
 *  $RCSfile: JobQueue.java,v $
 *
 *  $Revision: 1.16 $
 *
 *  last change: $Author: rt $ $Date: 2004-08-20 09:22:05 $
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


import java.util.Enumeration;
import java.util.Hashtable;


import com.sun.star.lib.util.IInvokeHook;
import com.sun.star.lib.util.IInvokable;


import com.sun.star.uno.UnoRuntime;



/**
 * The <code>JobQueue</code> implements a queue for jobs.
 * For every jobs thread id exists a job queue which is registered
 * at the <code>ThreadPool</code>.
 * A JobQueue is splitted in a sync job queue and an async job queue.
 * The sync job queue is the registerd queue, it delegates async jobs
 * (put by <code>putjob</code>) into the async queue, which is only 
 * known by the sync queue.
 * <p>
 * @version 	$Revision: 1.16 $ $ $Date: 2004-08-20 09:22:05 $
 * @author 	    Kay Ramme
 * @see         com.sun.star.lib.uno.environments.remote.ThreadPool
 * @see         com.sun.star.lib.uno.environments.remote.Job
 * @see         com.sun.star.lib.uno.environments.remote.ThreadID
 * @since       UDK1.0
 */
public class JobQueue {
    /**
     * When set to true, enables various debugging output.
     */
    private static final boolean DEBUG = false;

    /**
     * E.g. to get privleges for security managers, it is 
     * possible to set a hook for the <code>JobDispatcher</code> thread.
     */
    static public IInvokeHook __JobDispatcher_run_hook;

    protected Job _head;                 // the head of the job list
    protected Job _tail;                 // the tail of the job list

    protected ThreadId  _threadId;       // the thread id of the queue
    protected int       _ref_count = 0;  // the stack deepness
    protected boolean   _createThread;   // create a worker thread, if needed
    protected boolean   _createThread_now;   // create a worker thread, if needed
    protected Thread    _worker_thread;  // the thread that does the jobs

    protected Object    _disposeId; // the active dispose id
    protected Object    _doDispose = null;
    protected Throwable _throwable;

    protected JobQueue  _async_jobQueue; // chaining job qeueus for asyncs
    protected JobQueue  _sync_jobQueue;  // chaining job qeueus for syncs

    protected boolean _active = false;

    protected JavaThreadPoolFactory _javaThreadPoolFactory;

    /**
     * A thread for dispatching jobs
     */
    class JobDispatcher extends Thread implements IInvokable {
        Object _disposeId;

        JobDispatcher(Object disposeId) {
            if(DEBUG) System.err.println("JobQueue$JobDispatcher.<init>:" + _threadId);

            _disposeId = disposeId;
        }

        ThreadId getThreadId() {
            return _threadId;
        }

        public Object invoke(Object params[]) {
            try {
                  enter(2000, _disposeId);
            }
            catch(Throwable throwable) {
                if(_head != null || _active) { // there was a job in progress, so give a stack
                    System.err.println(getClass().getName() + " - exception occurred:" + throwable);
                    throwable.printStackTrace(System.err);
                }
            }
            finally {
                release();
            }

            return null;
        }
        
        public void run() {
            if(DEBUG) System.err.println("ThreadPool$JobDispatcher.run: " + Thread.currentThread());
        
            if(__JobDispatcher_run_hook != null) {
                try {
                    __JobDispatcher_run_hook.invoke(this, null);

                }
                catch(Exception exception) { // should not fly
                    System.err.println(getClass().getName() + " - unexpected: method >doWork< threw an exception - " + exception);
                }
            }
            else
                invoke(null);

            if(DEBUG) System.err.println("##### " + getClass().getName() + ".run - exit:" + _threadId);

//  			try {
//  				Object object = new Object();
//  				synchronized(object) {
//  					object.wait();
//  				}
//  			}
//  			catch(InterruptedException interruptedException) {
//  			}
        }
    }


    /**
     * Constructs a async job queue with the given thread id
     * which belongs to the given sync job queue.
     * <p>
     * @param threadId         the thread id
     * @param sync_jobQueue    the sync queue this async queue belongs to
     * @see                    com.sun.star.lib.uno.environments.remote.ThreadID
     */
    JobQueue(JavaThreadPoolFactory javaThreadPoolFactory, ThreadId threadId) {
        _javaThreadPoolFactory = javaThreadPoolFactory;
        _threadId = ThreadId.createFresh();

        _sync_jobQueue    = javaThreadPoolFactory.getJobQueue(threadId);
        if(_sync_jobQueue == null) {
            _sync_jobQueue = new JobQueue(javaThreadPoolFactory, threadId, true);
            _sync_jobQueue.acquire();
        }

        _sync_jobQueue._async_jobQueue = this;

        _createThread     = true;
        _createThread_now = true;

        acquire();

        if(DEBUG) System.err.println("##### " + getClass().getName() + " - init:" +  _threadId);
    }

    /**
     * Constructs a sync job queue with the given thread id and the given thread.
     * <p>
     * @param threadId        the thread id
     * @param createThread    if true, the queue creates a worker thread if needed
     * @see             com.sun.star.lib.uno.environments.remote.ThreadID
     */
    JobQueue(JavaThreadPoolFactory javaThreadPoolFactory, ThreadId threadId, boolean createThread){ 
        _javaThreadPoolFactory   = javaThreadPoolFactory;
        _threadId         = threadId;
        _createThread     = createThread;
        _createThread_now = createThread;

        if(DEBUG) System.err.println("##### " + getClass().getName() + " - init:" +  _threadId + " " + createThread);
    }

    /**
     * Gives the thread id of this queue
     * <p>
     * @return  the thread id
     * @see     com.sun.star.lib.uno.environments.remote.ThreadID
     */
    ThreadId getThreadId() {
        return _threadId;
    }

    synchronized void acquire() {
        // add only synchronous queues .
        if(_ref_count <= 0 && _sync_jobQueue == null ) 
            _javaThreadPoolFactory.addJobQueue(this);

        ++ _ref_count;
    }

    synchronized void release() {
        -- _ref_count;

        if(_ref_count <= 0) {
            // only synchronous queues needs to be removed .
            if( _sync_jobQueue == null ) 
                _javaThreadPoolFactory.removeJobQueue(this);


            if(_sync_jobQueue != null) {
                _sync_jobQueue._async_jobQueue = null;
                _sync_jobQueue.release();
            }
        }
    }

    /**
     * Removes a job from the queue.
     * <p>
     * @return a job or null if timed out
     * @param  waitTime        the maximum amount of time to wait for a job 
     */
    private Job removeJob(int waitTime) throws Throwable {
        if(DEBUG) System.err.println("##### " + getClass().getName() + ".removeJob:" + _head + " " + _threadId);

        Job job = null;
        synchronized (this) {
            // wait max. waitTime time for a job to enter the queue
            boolean waited = false;
            while(_head == null && (waitTime == 0 || !waited)) {
                if(_doDispose == _disposeId) {
                    _doDispose = null;
                    throw _throwable;
                }

                // notify sync queues
                notifyAll();

                try {
                    // wait for new job
                    wait(waitTime);
                }
                catch(InterruptedException interruptedException) {
                      throw new com.sun.star.uno.RuntimeException(getClass().getName() + ".removeJob - unexpected:" + interruptedException);
                }
                
                // signal that we have already waited once
                waited = true;
            }


            if(_head != null) {
                Job current = _head;
                _head    = _head._next;
                
                if(_head == null)
                    _tail = null;
                
                job = current;
                _active = true;
            }
        }

        // always wait for asynchron jobqueue to be finished !
        if(job != null && _async_jobQueue != null) {
            synchronized(_async_jobQueue) { 
                // wait for async queue to be empty and last job to be done
                while(_async_jobQueue._active || _async_jobQueue._head != null) {
                    if(DEBUG) System.err.println("waiting for async:" + _async_jobQueue._head + " " +  _async_jobQueue._worker_thread);

                    if(_doDispose == _disposeId) {
                        _doDispose = null;
                        throw _throwable;
                    }

                    try {
                        _async_jobQueue.wait();
                    }
                    catch(InterruptedException interruptedException) {
                        throw new com.sun.star.uno.RuntimeException(getClass().getName() + ".removeJob - unexpected:" + interruptedException);
                    }
                }
            }
        }

        return job;
    }

    /**
     * Puts a job into the queue.
     * <p>
     * @param  job        the job
     * @param  disposeId  a dispose id
     */
    synchronized void putJob(Job job, Object disposeId) {
        if(DEBUG) System.err.println("##### " + getClass().getName() + ".putJob todoes: " + " job:" + job);
        
        if(_tail != null) 
            _tail._next = job;
        else
            _head = job;
        
        _tail = job;

        if(_worker_thread == null && _createThread && _createThread_now) { // if there is no thread, which dispatches and if shall create one, create one

            acquire();

            _createThread_now = false;
            new JobDispatcher(disposeId).start();
        }

        // always notify possible waiters
        notifyAll();
    }

    /**
     * Enters the job queue.
     * <p>
     * @return the result of the final job (reply)
     * @param  disposeId  a dispose id
     */
    Object enter(Object disposeId) throws Throwable {
        return enter(0, disposeId); // wait infinitly
    }

    /**
     * Enters the job queue.
     * <p>
     * @return the result of the final job (reply)
     * @param  waitTime   the maximum amount of time to wait for a job (0 means wait infinitly)
     * @param  disposeId  a dispose id
     */
    Object enter(int waitTime, Object disposeId) throws Throwable {
        if(DEBUG) System.err.println("#####" + getClass().getName() + ".enter: " + _threadId);

        boolean quit = false;

        Object hold_disposeId = _disposeId;
        _disposeId = disposeId;

        Object result = null;

        Thread hold_worker_thread = _worker_thread;
        _worker_thread = Thread.currentThread();

        while(!quit) {
            Job job = null;
            
            try {
                job = removeJob(waitTime);
                
                if(job != null) {
                    try {
                        result = job.execute();
                    }
                    finally {
                        _active = false;
                    }

                    if(job.isFinal()) {
                        job.dispose();

                        quit = true;
                    }

                    job = null;
                }
                else
                    quit = true;

                
            }
            finally { // ensure that this queue becomes disposed, if necessary
                if(DEBUG) System.err.println("##### " + getClass().getName() + ".enter leaving: " + _threadId + " " + _worker_thread + " " + hold_worker_thread + " " + result);

                synchronized(this) {
                    if(job != null || (quit && _head == null)) {
                        _worker_thread = hold_worker_thread;

                        _createThread_now = true;
                    
                        _disposeId = hold_disposeId;

                        if(_sync_jobQueue != null)
                            notifyAll(); // notify waiters (e.g. this is an asyncQueue and there is a sync waiting)
                    }
                    else
                        quit = false;

                }
            }
        }

        return result;
    }

    /**
     * If the given disposeId is registered,
     * interrups the worker thread.
     * <p>
     * @param disposeId    the dispose id
     */
    synchronized void dispose(Object disposeId, Throwable throwable) {
        if(_sync_jobQueue == null) { // dispose only sync queues
            _doDispose = disposeId;
            _throwable = throwable;
            
            // get thread out of wait and let it throw the throwable
            if(DEBUG) System.err.println(getClass().getName() + ".dispose - notifying thread");
            
            notifyAll();
        }
    }
}

