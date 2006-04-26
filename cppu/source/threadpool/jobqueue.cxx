/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: jobqueue.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: kz $ $Date: 2006-04-26 20:49:42 $
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
#include "jobqueue.hxx"
#include "threadpool.hxx"

#include <osl/diagnose.h>

using namespace ::osl;

namespace cppu_threadpool {
    
    JobQueue::JobQueue(  sal_Bool bAsynchron ) :
        m_nToDo( 0 ),
        m_bSuspended( sal_False ),
        m_cndWait( osl_createCondition() )
    {
        osl_resetCondition( m_cndWait );		
    }
    
    JobQueue::~JobQueue()
    {
        osl_destroyCondition( m_cndWait );
    }
        
        
    void JobQueue::add( void *pThreadSpecificData ,
                        void ( SAL_CALL * doRequest ) ( void *pThreadSpecificData ) )
    {
        MutexGuard guard( m_mutex );
        Job job = { pThreadSpecificData , doRequest };
        m_lstJob.push_back( job );
        if( ! m_bSuspended )
        {
            osl_setCondition( m_cndWait );
        }
        m_nToDo ++;
    }
        
    void *JobQueue::enter( sal_Int64 nDisposeId , sal_Bool bReturnWhenNoJob )
    {
        void *pReturn = 0;
        {
            // synchronize with the dispose calls
            MutexGuard guard( m_mutex );
            if( DisposedCallerAdmin::getInstance()->isDisposed( nDisposeId ) )
            {
                return 0;
            }
            m_lstCallstack.push_front( nDisposeId );
        }

        
        while( sal_True )
        {
            if( bReturnWhenNoJob )
            {
                MutexGuard guard( m_mutex );
                if( m_lstJob.empty() )
                {
                    break;
                }
            }

            osl_waitCondition( m_cndWait , 0 );
            
            struct Job job={0,0};
            {
                // synchronize with add and dispose calls
                MutexGuard guard( m_mutex );

                if( 0 == m_lstCallstack.front() )
                {
                    // disposed !
                    break;
                }

                OSL_ASSERT( ! m_lstJob.empty() );
                if( ! m_lstJob.empty() )
                {
                    job = m_lstJob.front();
                    m_lstJob.pop_front();
                }
                if( m_lstJob.empty() )
                {
                    osl_resetCondition( m_cndWait );
                }
            }

            if( job.doRequest )
            {
                job.doRequest( job.pThreadSpecificData );
                m_nToDo --;
            }
            else
            {
                m_nToDo --;
                pReturn = job.pThreadSpecificData;
                break;
            }
        }

        {
            // synchronize with the dispose calls
            MutexGuard guard( m_mutex );
            m_lstCallstack.pop_front();
        }
        
        return pReturn;
    }
    
    void JobQueue::dispose( sal_Int64 nDisposeId )
    {
        MutexGuard guard( m_mutex );
        for( CallStackList::iterator ii = m_lstCallstack.begin() ;
             ii != m_lstCallstack.end() ;
             ++ii )
        {
            if( (*ii) == nDisposeId )
            {
                (*ii) = 0;
            }
        }

        if( !m_lstCallstack.empty()  && ! m_lstCallstack.front() )
        {
            // The thread is waiting for a disposed pCallerId, let it go
            osl_setCondition( m_cndWait );
        }
    }
        
    void JobQueue::suspend()
    {
        MutexGuard guard( m_mutex );
        m_bSuspended = sal_True;
    }

    void JobQueue::resume()
    {
        MutexGuard guard( m_mutex );
        m_bSuspended = sal_False;
        if( ! m_lstJob.empty() )
        {
            osl_setCondition( m_cndWait );
        }
    }

    sal_Bool JobQueue::isEmpty()
    {
        MutexGuard guard( m_mutex );
        return m_lstJob.empty();
    }

    sal_Bool JobQueue::isCallstackEmpty()
    {
        MutexGuard guard( m_mutex );
        return m_lstCallstack.empty();
    }

    sal_Bool JobQueue::isBusy()
    {
        return m_nToDo > 0;
    }


}
