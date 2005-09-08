/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: mutex.hxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 14:29:23 $
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

#ifndef _OSL_MUTEX_HXX_
#define _OSL_MUTEX_HXX_

#ifdef __cplusplus

#include <osl/mutex.h>


namespace osl
{     
    /** A mutual exclusion synchronization object
    */
    class Mutex {
    
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
        
    private:
        oslMutex mutex;

        /** The underlying oslMutex has no reference count.
        
        Since the underlying oslMutex is not a reference counted object, copy
        constructed Mutex may work on an already destructed oslMutex object.
        
        */
        Mutex(const Mutex&);
        
        /** The underlying oslMutex has no reference count.
        
        When destructed, the Mutex object destroys the undelying oslMutex, 
        which might cause severe problems in case it's a temporary object.
         
        */
        Mutex(oslMutex Mutex);
        
        /** This assignment operator is private for the same reason as
            the copy constructor.
        */
        Mutex& operator= (const Mutex&);
        
        /** This assignment operator is private for the same reason as
            the constructor taking a oslMutex argument.
        */
        Mutex& operator= (oslMutex);
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
                this->pT = pResetT;
                this->pT->acquire();
            }
        }
    };

    typedef Guard<Mutex> MutexGuard;
    typedef ClearableGuard<Mutex> ClearableMutexGuard;
    typedef ResettableGuard< Mutex > ResettableMutexGuard;
}

#endif  /* __cplusplus */
#endif	/* _OSL_MUTEX_HXX_ */

