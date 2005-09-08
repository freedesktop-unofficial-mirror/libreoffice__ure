/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: mutex.c,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 15:07:39 $
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

#include <osl/mutex.h>
#include <osl/diagnose.h>

/*
    Implementation notes:
    The void* hidden by oslMutex points to a WIN32
    CRITICAL_SECTION structure.
*/

typedef struct _oslMutexImpl {
    CRITICAL_SECTION	m_Mutex;
    int 				m_Locks;
    DWORD				m_Owner;
    DWORD				m_Requests;
} oslMutexImpl;

static BOOL (WINAPI *lpfTryEnterCriticalSection)(LPCRITICAL_SECTION)
    = (BOOL (WINAPI *)(LPCRITICAL_SECTION))0xFFFFFFFF;

static CRITICAL_SECTION MutexLock;

/*****************************************************************************/
/* osl_createMutex */
/*****************************************************************************/
oslMutex SAL_CALL osl_createMutex(void)
{
    oslMutexImpl *pMutexImpl;

    /* Window 95 does not support "TryEnterCriticalSection" */

    if (lpfTryEnterCriticalSection ==
                (BOOL (WINAPI *)(LPCRITICAL_SECTION))0xFFFFFFFF)
    {
        OSVERSIONINFO VersionInformation = 

        {
            sizeof(OSVERSIONINFO),
            0, 
            0, 
            0,
            0, 
            "",
        };

        /* ts: Window 98 does not support "TryEnterCriticalSection" but export the symbol !!!
           calls to that symbol always returns FALSE */
        if (
            GetVersionEx(&VersionInformation) &&
            (VersionInformation.dwPlatformId == VER_PLATFORM_WIN32_NT)
           )
        {
            lpfTryEnterCriticalSection = (BOOL (WINAPI *)(LPCRITICAL_SECTION))
                    GetProcAddress(GetModuleHandle("KERNEL32"),
                                   "TryEnterCriticalSection");
        }
        else
        {
            lpfTryEnterCriticalSection = (BOOL (WINAPI *)(LPCRITICAL_SECTION))NULL;
        }


        InitializeCriticalSection(&MutexLock);
    }

    pMutexImpl= calloc(sizeof(oslMutexImpl), 1);

    OSL_ASSERT(pMutexImpl); /* alloc successful? */

    InitializeCriticalSection(&pMutexImpl->m_Mutex);

    return (oslMutex)pMutexImpl;
}

/*****************************************************************************/
/* osl_destroyMutex */
/*****************************************************************************/
void SAL_CALL osl_destroyMutex(oslMutex Mutex)
{
    oslMutexImpl *pMutexImpl = (oslMutexImpl *)Mutex;

    if (pMutexImpl)
    {
        DeleteCriticalSection(&pMutexImpl->m_Mutex);
        free(pMutexImpl);
    }
}

/*****************************************************************************/
/* osl_acquireMutex */
/*****************************************************************************/
sal_Bool SAL_CALL osl_acquireMutex(oslMutex Mutex)
{
    oslMutexImpl *pMutexImpl = (oslMutexImpl *)Mutex;

    OSL_ASSERT(Mutex);

    if (lpfTryEnterCriticalSection == NULL)
    {
        EnterCriticalSection(&MutexLock);
        pMutexImpl->m_Requests++;
        LeaveCriticalSection(&MutexLock);

        EnterCriticalSection(&pMutexImpl->m_Mutex);

        EnterCriticalSection(&MutexLock);
        pMutexImpl->m_Requests--;
        if (pMutexImpl->m_Locks++ == 0)
            pMutexImpl->m_Owner = GetCurrentThreadId();
        LeaveCriticalSection(&MutexLock);
    }
    else
        EnterCriticalSection(&pMutexImpl->m_Mutex);

    return sal_True;
}

/*****************************************************************************/
/* osl_tryToAcquireMutex */
/*****************************************************************************/
sal_Bool SAL_CALL osl_tryToAcquireMutex(oslMutex Mutex)
{
    sal_Bool 	 ret = sal_False;
    oslMutexImpl *pMutexImpl = (oslMutexImpl *)Mutex;

    OSL_ASSERT(Mutex);

    if (lpfTryEnterCriticalSection != NULL)
        return (sal_Bool)(lpfTryEnterCriticalSection(&pMutexImpl->m_Mutex) != FALSE);
    else
    {
        EnterCriticalSection(&MutexLock);

        if ( ((pMutexImpl->m_Requests == 0) && (pMutexImpl->m_Locks == 0)) ||
             (pMutexImpl->m_Owner == GetCurrentThreadId()) )
            ret = osl_acquireMutex(Mutex);

        LeaveCriticalSection(&MutexLock);
    }

    return ret;
}

/*****************************************************************************/
/* osl_releaseMutex */
/*****************************************************************************/
sal_Bool SAL_CALL osl_releaseMutex(oslMutex Mutex)
{
    oslMutexImpl *pMutexImpl = (oslMutexImpl *)Mutex;

    OSL_ASSERT(Mutex);

    if (lpfTryEnterCriticalSection == NULL)
    {
        EnterCriticalSection(&MutexLock);

        if (--(pMutexImpl->m_Locks) == 0)
            pMutexImpl->m_Owner = 0;

        LeaveCriticalSection(&MutexLock);
    }

    LeaveCriticalSection(&pMutexImpl->m_Mutex);

    return sal_True;
}

/*****************************************************************************/
/* osl_getGlobalMutex */
/*****************************************************************************/

/* initialized in dllentry.c */
oslMutex g_Mutex;

oslMutex * SAL_CALL osl_getGlobalMutex(void) 
{
    return &g_Mutex;
}
