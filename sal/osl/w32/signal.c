/*************************************************************************
 *
 *  $RCSfile: signal.c,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: hr $ $Date: 2004-02-03 13:33:53 $
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

/* system headers */
#include "system.h"
#include <tchar.h>

#include <osl/diagnose.h>
#include <osl/mutex.h>
#include <osl/signal.h>
#include <DbgHelp.h>
#include <ErrorRep.h>
#include <systools/win32/uwinapi.h>

typedef struct _oslSignalHandlerImpl
{
    oslSignalHandlerFunction      Handler;
    void*			        	  pData;
    struct _oslSignalHandlerImpl* pNext;	
} oslSignalHandlerImpl;

static sal_Bool  			  bInitSignal = sal_False;
static oslMutex 			  SignalListMutex;
static oslSignalHandlerImpl*  SignalList;

static long WINAPI SignalHandlerFunction(LPEXCEPTION_POINTERS lpEP);

static sal_Bool InitSignal(void)
{
    HMODULE	hFaultRep;

    SignalListMutex = osl_createMutex();

    SetUnhandledExceptionFilter(SignalHandlerFunction);

    hFaultRep = LoadLibrary( "faultrep.dll" );
    if ( hFaultRep )
    {
        pfn_ADDEREXCLUDEDAPPLICATIONW		pfn = (pfn_ADDEREXCLUDEDAPPLICATIONW)GetProcAddress( hFaultRep, "AddERExcludedApplicationW" );
        if ( pfn )
            pfn( L"SOFFICE.EXE" );
        FreeLibrary( hFaultRep );
    }

    return sal_True;
}

static sal_Bool DeInitSignal(void)
{
    SetUnhandledExceptionFilter(NULL);

    osl_destroyMutex(SignalListMutex);

    return sal_False;
}

static oslSignalAction CallSignalHandler(oslSignalInfo *pInfo)
{
    oslSignalHandlerImpl* pHandler = SignalList;
    oslSignalAction Action = osl_Signal_ActCallNextHdl;

    while (pHandler != NULL)
    {
        if ((Action = pHandler->Handler(pHandler->pData, pInfo)) != osl_Signal_ActCallNextHdl)
            break;

        pHandler = pHandler->pNext;
    }

    return Action;
}

/*****************************************************************************/
/* SignalHandlerFunction	*/
/*****************************************************************************/

static BOOL ReportCrash( LPEXCEPTION_POINTERS lpEP )
{
    BOOL	fSuccess = FALSE;
    TCHAR	szBuffer[1024];
    TCHAR	szPath[MAX_PATH];
    LPTSTR	lpFilePart;
    PROCESS_INFORMATION	ProcessInfo;
    STARTUPINFO	StartupInfo;
    int		argi;

    /* Check if crash reporter was disabled by command line */

    for ( argi = 1; argi < __argc; argi++ )
    {
        if ( 
            0 == stricmp( __argv[argi], "-nocrashreport" ) || 
            0 == stricmp( __argv[argi], "/nocrashreport" )
            )
            return FALSE;
    }

    if ( SearchPath( NULL, TEXT("crashrep.exe"), NULL, MAX_PATH, szPath, &lpFilePart ) )
    {
        ZeroMemory( &StartupInfo, sizeof(StartupInfo) );
        StartupInfo.cb = sizeof(StartupInfo.cb);


        sntprintf( szBuffer, elementsof(szBuffer), 
            _T("%s -p %u -excp 0x%p -t %u"), 
            szPath, 
            GetCurrentProcessId(), 
            lpEP, 
            GetCurrentThreadId() );

        if ( 
            CreateProcess( 
                NULL, 
                szBuffer, 
                NULL, 
                NULL, 
                FALSE, 
#ifdef UNICODE
                CREATE_UNICODE_ENVIRONMENT, 
#else
                0,
#endif
                NULL, NULL, &StartupInfo, &ProcessInfo ) 
            )
        {
            DWORD	dwExitCode;

            WaitForSingleObject( ProcessInfo.hProcess, INFINITE );
            if ( GetExitCodeProcess( ProcessInfo.hProcess, &dwExitCode ) && 0 == dwExitCode )
            
            fSuccess = TRUE;

        }
    }

    return fSuccess;
}

/*****************************************************************************/
/* SignalHandlerFunction	*/
/*****************************************************************************/

static BOOL WINAPI IsWin95A(void)
{
    OSVERSIONINFO	ovi;

    ZeroMemory( &ovi, sizeof(ovi) );
    ovi.dwOSVersionInfoSize = sizeof(ovi);

    if ( GetVersionEx( &ovi ) )
        /* See MSDN January 2000 documentation of GetVersionEx */
        return	(ovi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && 
                (ovi.dwMajorVersion <= 4) && 
                (ovi.dwMinorVersion == 0) && 
                (ovi.dwBuildNumber == 0x040003B6);

    /* Something wrent wrong. So assume we have an older operating prior Win95 */

    return TRUE;
}

/* magic Microsoft C++ compiler exception constant */
#define EXCEPTION_MSC_CPP_EXCEPTION 0xe06d7363

static long WINAPI SignalHandlerFunction(LPEXCEPTION_POINTERS lpEP)
{
    static sal_Bool		bNested = sal_False;
    sal_Bool		bRaiseCrashReporter = sal_False;
    oslSignalInfo	Info;
    oslSignalAction	Action;

    Info.UserSignal = lpEP->ExceptionRecord->ExceptionCode;
    Info.UserData   = NULL;

    switch (lpEP->ExceptionRecord->ExceptionCode)
    {
        /* Transform unhandled exceptions into access violations.
           Microsoft C++ compiler (add more for other compilers if necessary).
         */
        case EXCEPTION_MSC_CPP_EXCEPTION:
        case EXCEPTION_ACCESS_VIOLATION: 
            Info.Signal = osl_Signal_AccessViolation;
            bRaiseCrashReporter = sal_True;
            break; 

        case EXCEPTION_INT_DIVIDE_BY_ZERO: 
            Info.Signal = osl_Signal_IntegerDivideByZero;
            bRaiseCrashReporter = sal_True;
            break; 

        case EXCEPTION_FLT_DIVIDE_BY_ZERO: 
            Info.Signal = osl_Signal_FloatDivideByZero;
            bRaiseCrashReporter = sal_True;
            break; 

        case EXCEPTION_BREAKPOINT: 
            Info.Signal = osl_Signal_DebugBreak;
            break; 
            
        default:
            Info.Signal = osl_Signal_System;
            bRaiseCrashReporter = sal_True;
            break; 
    }

    if ( !bNested )
    {
        bNested = sal_True;

        if ( bRaiseCrashReporter && ReportCrash( lpEP ) || IsWin95A() )
        {
            CallSignalHandler(&Info);
            Action = osl_Signal_ActKillApp;
        }
        else
            Action = CallSignalHandler(&Info);
    }
    else
        Action = osl_Signal_ActKillApp;


    switch ( Action )
    {
        case osl_Signal_ActCallNextHdl:
            return (EXCEPTION_CONTINUE_SEARCH);

        case osl_Signal_ActAbortApp:
            return (EXCEPTION_EXECUTE_HANDLER);

        case osl_Signal_ActKillApp:
            SetErrorMode(SEM_NOGPFAULTERRORBOX);
            exit(255);
            break;
    }

    return (EXCEPTION_CONTINUE_EXECUTION);
}

/*****************************************************************************/
/* osl_addSignalHandler */
/*****************************************************************************/
oslSignalHandler SAL_CALL osl_addSignalHandler(oslSignalHandlerFunction Handler, void* pData)
{
    oslSignalHandlerImpl* pHandler;

    OSL_ASSERT(Handler != NULL);

    if (! bInitSignal)
        bInitSignal = InitSignal();

    pHandler = calloc(1, sizeof(oslSignalHandlerImpl));

    if (pHandler != NULL)
    {	
        pHandler->Handler = Handler;
        pHandler->pData   = pData;

        osl_acquireMutex(SignalListMutex);
    
        pHandler->pNext = SignalList;
        SignalList      = pHandler;

        osl_releaseMutex(SignalListMutex);

        return (pHandler);
    }

    return (NULL);
}

/*****************************************************************************/
/* osl_removeSignalHandler */
/*****************************************************************************/
sal_Bool SAL_CALL osl_removeSignalHandler(oslSignalHandler Handler)
{
    oslSignalHandlerImpl *pHandler, *pPrevious = NULL;

    OSL_ASSERT(Handler != NULL);

    if (! bInitSignal)
        bInitSignal = InitSignal();

    osl_acquireMutex(SignalListMutex);

    pHandler = SignalList;

    while (pHandler != NULL)
    {
        if (pHandler == Handler)
        {
            if (pPrevious)
                pPrevious->pNext = pHandler->pNext;
            else
                SignalList = pHandler->pNext;

            osl_releaseMutex(SignalListMutex);

            if (SignalList == NULL)
                bInitSignal = DeInitSignal();

            free(pHandler);

            return (sal_True);
        }

        pPrevious = pHandler;
        pHandler  = pHandler->pNext;
    }
            
    osl_releaseMutex(SignalListMutex);

    return (sal_False);
}

/*****************************************************************************/
/* osl_raiseSignal */
/*****************************************************************************/
oslSignalAction SAL_CALL osl_raiseSignal(sal_Int32 UserSignal, void* UserData)
{
    oslSignalInfo   Info;
    oslSignalAction Action;

    if (! bInitSignal)
        bInitSignal = InitSignal();

    osl_acquireMutex(SignalListMutex);

    Info.Signal     = osl_Signal_User;
    Info.UserSignal = UserSignal;
    Info.UserData   = UserData;

    Action = CallSignalHandler(&Info);
            
    osl_releaseMutex(SignalListMutex);

    return (Action);
}

