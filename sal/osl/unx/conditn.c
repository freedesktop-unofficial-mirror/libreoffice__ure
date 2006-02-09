/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: conditn.c,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2006-02-09 17:06:13 $
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


#include "system.h"

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#include <osl/conditn.h>
#include <osl/diagnose.h>

#ifndef _OSL_TIME_H_
#include <osl/time.h>
#endif


typedef struct _oslConditionImpl
{
    pthread_cond_t	m_Condition;
    pthread_mutex_t	m_Lock;
    sal_Bool			m_State;
} oslConditionImpl;


/*****************************************************************************/
/* osl_createCondition */
/*****************************************************************************/
oslCondition SAL_CALL osl_createCondition()
{
      oslConditionImpl* pCond;
    int nRet=0;
    
    pCond = (oslConditionImpl*) malloc(sizeof(oslConditionImpl));

    OSL_ASSERT(pCond);

    if ( pCond == 0 )
    {
        return 0;
    }
    
    pCond->m_State = sal_False;

    /* init condition variable with default attr. (PTHREAD_PROCESS_PRIVAT) */
    nRet =  pthread_cond_init(&pCond->m_Condition, PTHREAD_CONDATTR_DEFAULT);
    if ( nRet != 0 )
    {
        OSL_TRACE("osl_createCondition : condition init failed. Errno: %d; '%s'\n",
                  nRet, strerror(nRet));

        free(pCond);
        return 0;
    }

    nRet = pthread_mutex_init(&pCond->m_Lock, PTHREAD_MUTEXATTR_DEFAULT);
    if ( nRet != 0 )
    {
        OSL_TRACE("osl_createCondition : mutex init failed. Errno: %d; %s\n",
                  nRet, strerror(nRet));
        
        nRet = pthread_cond_destroy(&pCond->m_Condition);
        if ( nRet != 0 )
        {
            OSL_TRACE("osl_createCondition : destroy condition failed. Errno: %d; '%s'\n",
                      nRet, strerror(nRet));
        }

        free(pCond);
        pCond = 0;
    }

    return (oslCondition)pCond;
}

/*****************************************************************************/
/* osl_destroyCondition */
/*****************************************************************************/
void SAL_CALL osl_destroyCondition(oslCondition Condition)
{
    oslConditionImpl* pCond;
    int nRet = 0;
    
    if ( Condition )
    {
        pCond = (oslConditionImpl*)Condition;

        nRet = pthread_cond_destroy(&pCond->m_Condition);
        if ( nRet != 0 )
        {
            OSL_TRACE("osl_destroyCondition : destroy condition failed. Errno: %d; '%s'\n",
                      nRet, strerror(nRet));
        }
        nRet = pthread_mutex_destroy(&pCond->m_Lock);
        if ( nRet != 0 )
        {
            OSL_TRACE("osl_destroyCondition : destroy mutex failed. Errno: %d; '%s'\n",
                      nRet, strerror(nRet));            
        }

        free(Condition);
    }
    
    return;
}

/*****************************************************************************/
/* osl_setCondition */
/*****************************************************************************/
sal_Bool SAL_CALL osl_setCondition(oslCondition Condition)
{
   oslConditionImpl* pCond;
   int nRet=0;
   
   OSL_ASSERT(Condition);
   pCond = (oslConditionImpl*)Condition;

   if ( pCond == 0 )
   {
       return sal_False;
   }

   nRet = pthread_mutex_lock(&pCond->m_Lock);
   if ( nRet != 0 )
   {
       OSL_TRACE("osl_setCondition : mutex lock failed. Errno: %d; %s\n",
                  nRet, strerror(nRet));
       return sal_False;
   }

   pCond->m_State = sal_True;
   nRet = pthread_cond_broadcast(&pCond->m_Condition);
   if ( nRet != 0 )
   {
       OSL_TRACE("osl_setCondition : condition broadcast failed. Errno: %d; %s\n",
                  nRet, strerror(nRet));
       return sal_False;
   }

   nRet = pthread_mutex_unlock(&pCond->m_Lock);
   if ( nRet != 0 )
   {
       OSL_TRACE("osl_setCondition : mutex unlock failed. Errno: %d; %s\n",
                  nRet, strerror(nRet));
       return sal_False;
   }
   
   return sal_True;

}

/*****************************************************************************/
/* osl_resetCondition */
/*****************************************************************************/
sal_Bool SAL_CALL osl_resetCondition(oslCondition Condition)
{
    oslConditionImpl* pCond;
    int nRet=0;
    
    OSL_ASSERT(Condition);

    pCond = (oslConditionImpl*)Condition;

    if ( pCond == 0 )
    {
        return sal_False;
    }

    nRet = pthread_mutex_lock(&pCond->m_Lock);
    if ( nRet != 0 )
    {
       OSL_TRACE("osl_resetCondition : mutex lock failed. Errno: %d; %s\n",
                  nRet, strerror(nRet));
        return sal_False;
    }

    pCond->m_State = sal_False;

    nRet = pthread_mutex_unlock(&pCond->m_Lock);
    if ( nRet != 0 )
    {
       OSL_TRACE("osl_resetCondition : mutex unlock failed. Errno: %d; %s\n",
                  nRet, strerror(nRet));
        return sal_False;
    }
    
    return sal_True;
}

/*****************************************************************************/
/* osl_waitCondition */
/*****************************************************************************/
oslConditionResult SAL_CALL osl_waitCondition(oslCondition Condition, const TimeValue* pTimeout)
{
    oslConditionImpl* pCond;
    int nRet=0;
    oslConditionResult Result = osl_cond_result_ok;

    OSL_ASSERT(Condition);
    pCond = (oslConditionImpl*)Condition;

    if ( pCond == 0 )
    {
        return osl_cond_result_error; 
    }

    nRet = pthread_mutex_lock(&pCond->m_Lock);
    if ( nRet != 0 )
    {
       OSL_TRACE("osl_waitCondition : mutex lock failed. Errno: %d; %s\n",
                  nRet, strerror(nRet));
        return osl_cond_result_error;
    }

    if ( ! pCond->m_State )
    {
        if ( pTimeout )
        {
            int					ret;
            struct timeval      tp;
            struct timespec		to;
            
            gettimeofday(&tp, NULL);

            SET_TIMESPEC( to, tp.tv_sec + pTimeout->Seconds,
                              tp.tv_usec * 1000 + pTimeout->Nanosec );

            /* spurious wake up prevention */
            do
            {
                ret = pthread_cond_timedwait(&pCond->m_Condition, &pCond->m_Lock, &to);
                if ( ret != 0 )
                {
                    if ( ret == ETIME || ret == ETIMEDOUT )
                    {
                        Result = osl_cond_result_timeout;
                        nRet = pthread_mutex_unlock(&pCond->m_Lock);                        
                        if (nRet != 0)    
                        {
                            OSL_TRACE("osl_waitCondition : mutex unlock failed. Errno: %d; %s\n",
                                      nRet, strerror(nRet));
                        }
                        
                        return Result;
                    }
                    else if ( ret != EINTR )
                    {
                        Result = osl_cond_result_error;
                        nRet = pthread_mutex_unlock(&pCond->m_Lock);                        
                        if ( nRet != 0 )    
                        {
                            OSL_TRACE("osl_waitCondition : mutex unlock failed. Errno: %d; %s\n",
                                      nRet, strerror(nRet));
                        }
                        return Result;
                    }
/*                    OSL_TRACE("EINTR\n");*/
                }
            }
            while ( ret != 0 );
        }
        else
        {
            /* spurious wake up prevention */
            do
            {                
                nRet = pthread_cond_wait(&pCond->m_Condition, &pCond->m_Lock);
            }
            while ( nRet != 0 );
                
            if ( nRet != 0 )
            {
                OSL_TRACE("osl_waitCondition : condition wait failed. Errno: %d; %s\n",
                          nRet, strerror(nRet));
                Result = osl_cond_result_error;
                nRet = pthread_mutex_unlock(&pCond->m_Lock);
                if ( nRet != 0 )
                {
                    OSL_TRACE("osl_waitCondition : mutex unlock failed. Errno: %d; %s\n",
                              nRet, strerror(nRet));
                }
                
                return Result;
            }
        }
    }

    nRet = pthread_mutex_unlock(&pCond->m_Lock);
    if ( nRet != 0 )
    {
        OSL_TRACE("osl_waitCondition : mutex unlock failed. Errno: %d; %s\n",
                  nRet, strerror(nRet));
    }

    return Result;
}

/*****************************************************************************/
/* osl_checkCondition */
/*****************************************************************************/
sal_Bool SAL_CALL osl_checkCondition(oslCondition Condition)
{
    sal_Bool State;
    oslConditionImpl* pCond;
    int nRet=0;
    
    OSL_ASSERT(Condition);
    pCond = (oslConditionImpl*)Condition;

    if ( pCond == 0 )
    {
        return sal_False;
    }
    
    nRet = pthread_mutex_lock(&pCond->m_Lock);
    if ( nRet != 0 )
    {
        OSL_TRACE("osl_checkCondition : mutex unlock failed. Errno: %d; %s\n",
                  nRet, strerror(nRet));        
    }
    
    State = pCond->m_State;

    nRet = pthread_mutex_unlock(&pCond->m_Lock);
    if ( nRet != 0 )
    {
        OSL_TRACE("osl_checkCondition : mutex unlock failed. Errno: %d; %s\n",
                  nRet, strerror(nRet));
    }

    return State;
}


