/*************************************************************************
 *
 *  $RCSfile: mutex.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: hr $ $Date: 2003-04-04 17:10:54 $
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

#ifndef _OSL_MUTEX_HXX_
#define _OSL_MUTEX_HXX_

#ifdef __cplusplus

#include <osl/mutex.h>


namespace osl
{     
    /** A mutual exclusion synchronization object
    */
    class Mutex {
        oslMutex mutex;

        // these make no sense
        Mutex( oslMutex );
        Mutex( const Mutex & );
        Mutex& operator= ( oslMutex );
        Mutex& operator= ( const Mutex& );
        
    public:
        /** Create a thread-local mutex.
            @return 0 if the mutex could not be created, otherwise a handle to the mutex.
            @seealso ::osl_createMutex()
        */
        Mutex() 
        {
            mutex = osl_createMutex();
        }

        /** Release the OS-structures and free mutex data-structure. 
            @seealso ::osl_destroyMutex()
        */
        ~Mutex()
        {
            osl_destroyMutex(mutex);
        }

        /** Acquire the mutex, block if already acquired by another thread.
            @return sal_False if system-call fails.
            @seealso ::osl_acquireMutex()
        */
        sal_Bool acquire()
        {
            return osl_acquireMutex(mutex);
        }

        /** Try to acquire the mutex without blocking.
            @return sal_False if it could not be acquired.
            @seealso ::osl_tryToAcquireMutex()
        */
        sal_Bool tryToAcquire()
        {
            return osl_tryToAcquireMutex(mutex);
        }

        /** Release the mutex.
            @return sal_False if system-call fails.
            @seealso ::osl_releaseMutex()
        */
        sal_Bool release()
        {
            return osl_releaseMutex(mutex);
        }

        /** Returns a global static mutex object.
            The global and static mutex object can be used to initialize other 
            static objects in a thread safe manner.
            @return the global mutex object
            @seealso ::osl_getGlobalMutex()
        */
        static Mutex * getGlobalMutex()
        {
            return (Mutex *)osl_getGlobalMutex();
        }
    };

    /** A helper class for mutex objects and interfaces.
    */
    template<class T>
    class Guard
    {
    private:
        Guard( const Guard& );
        const Guard& operator = ( const Guard& );

    protected:
        T * pT;
    public:

        /** Acquires the object specified as parameter.
        */
        Guard(T * pT_) : pT(pT_)
        {
            pT->acquire();
        }

        /** Acquires the object specified as parameter.
        */
        Guard(T & t) : pT(&t)
        {
            pT->acquire();
        }

        /** Releases the mutex or interface. */
        ~Guard()
        {
            pT->release();
        }
    };

    /** A helper class for mutex objects and interfaces.
    */
    template<class T>
    class ClearableGuard
    {
    private:
        ClearableGuard( const ClearableGuard& );
        const ClearableGuard& operator = ( const ClearableGuard& );
    protected:
        T * pT;
    public:
        
        /** Acquires the object specified as parameter.
        */
        ClearableGuard(T * pT_) : pT(pT_)
        {
            pT->acquire();
        }

        /** Acquires the object specified as parameter.
        */
        ClearableGuard(T & t) : pT(&t)
        {
            pT->acquire();
        }

        /** Releases the mutex or interface if not already released by clear().
        */
        ~ClearableGuard()
        {
            if (pT)
                pT->release();
        }
        
        /** Releases the mutex or interface. 
        */
        void clear()
        {
            if(pT)
            {
                pT->release();
                pT = NULL;
            }
        }
    };

    /** A helper class for mutex objects and interfaces.
    */
    template< class T >
    class ResettableGuard : public ClearableGuard< T >
    {
    protected:
        T* pResetT;
    public:
        /** Acquires the object specified as parameter.
        */
        ResettableGuard( T* pT ) :
                ClearableGuard<T>( pT ),
                pResetT( pT )
        {}
        
        /** Acquires the object specified as parameter.
        */
        ResettableGuard( T& rT ) :
                ClearableGuard<T>( rT ),
                pResetT( &rT )
        {}
        
        /** Re-aquires the mutex or interface.
        */
        void reset()
        {
            if( pResetT )
            {
                pT = pResetT;
                pT->acquire();
            }
        }
    };

    typedef Guard<Mutex> MutexGuard;
    typedef ClearableGuard<Mutex> ClearableMutexGuard;
    typedef ResettableGuard< Mutex > ResettableMutexGuard;
}

#endif  /* __cplusplus */
#endif	/* _OSL_MUTEX_HXX_ */

