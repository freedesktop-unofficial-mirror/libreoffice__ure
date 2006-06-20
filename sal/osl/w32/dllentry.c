/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dllentry.c,v $
 *
 *  $Revision: 1.26 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-20 04:20:20 $
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

#pragma warning(push,1) /* disable warnings within system headers */
#include <windows.h>
#pragma warning(pop)
#include <tlhelp32.h>
#include <systools/win32/uwinapi.h>
#include <winsock.h>
#include <osl/diagnose.h>
#include <sal/types.h>
#include <float.h>

#include <osl/diagnose.h>
#include <osl/mutex.h>
#include <sal/types.h>

//------------------------------------------------------------------------------
// externals
//------------------------------------------------------------------------------

extern HRESULT (WINAPI *_CoInitializeEx) (LPVOID pvReserved, DWORD dwCoInit);

extern DWORD			g_dwTLSTextEncodingIndex;
extern void SAL_CALL	_osl_callThreadKeyCallbackOnThreadDetach(void);
extern CRITICAL_SECTION	g_ThreadKeyListCS;
extern oslMutex			g_Mutex;
extern oslMutex			g_CurrentDirectoryMutex;

extern void rtl_memory_fini (void);
extern void rtl_cache_fini (void);
extern void rtl_arena_fini (void);

/* 
This is needed because DllMain is called after static constructors. A DLL's
startup and shutdown sequence looks like this:

_pRawDllMain()
_CRT_INIT()
DllMain()
....
DllMain()
_CRT_INIT()
_pRawDllMain()

*/


static BOOL WINAPI _RawDllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved );
extern BOOL (WINAPI *_pRawDllMain)(HANDLE, DWORD, LPVOID) = _RawDllMain;

//------------------------------------------------------------------------------
// defines
//------------------------------------------------------------------------------

#define ERR_GENERAL_WRONG_CPU		101
#define ERR_WINSOCK_INIT_FAILED		102
#define ERR_WINSOCK_WRONG_VERSION	103			
#define ERR_NO_DCOM_UPDATE          104

//------------------------------------------------------------------------------
// globales
//------------------------------------------------------------------------------

DWORD         g_dwPlatformId = VER_PLATFORM_WIN32_WINDOWS; // remember plattform

//------------------------------------------------------------------------------
// showMessage
//------------------------------------------------------------------------------

static sal_Bool showMessage(int MessageId)
{
    const char *pStr = "unknown error";

    switch ( MessageId )
    {
        case ERR_GENERAL_WRONG_CPU:
            pStr = "x486 or Pentium compatible CPU required!\nThe application may not run stable.";
            break;

        case ERR_WINSOCK_INIT_FAILED:
            pStr = "Failed to initialize WINSOCK library!\nThe application may not run stable.";
            break;

        case ERR_WINSOCK_WRONG_VERSION:			
            pStr = "Wrong version of WINSOCK library!\nThe application may not run stable.";
            break;

        case ERR_NO_DCOM_UPDATE:
            pStr = "No DCOM update installed! The application may not run stable.\nPlease read the readme file for the necessary system requirements.";
            break;

        default:
            pStr = "Unknown error while initialization!\nThe application may not run stable.";
    }

    MessageBox( NULL, 
                pStr, 
                "OpenOffice.org - System Abstraction Layer", 
                MB_OK | MB_ICONWARNING | MB_TASKMODAL );

    return ( sal_True );
}

//------------------------------------------------------------------------------
// InitDCOM
//------------------------------------------------------------------------------

static void InitDCOM(void)
{
    HINSTANCE hInstance = GetModuleHandle( "ole32.dll" );

    if( hInstance )
    {
        FARPROC pFunc = GetProcAddress( hInstance, "CoInitializeEx" );

        if( pFunc )
            _CoInitializeEx = ( HRESULT ( WINAPI * ) ( LPVOID, DWORD ) ) pFunc;
        else
            showMessage( ERR_NO_DCOM_UPDATE );
    }
}

//------------------------------------------------------------------------------
// DllMain
//------------------------------------------------------------------------------

static BOOL WINAPI _RawDllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved )
{
    /* avoid warnings */
    hinstDLL = hinstDLL;     
    lpvReserved = lpvReserved; 
    
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            {
                OSVERSIONINFO aInfo;				
                WSADATA wsaData;
                int     error;
                WORD    wVersionRequested;

#ifdef _M_IX86
                SYSTEM_INFO SystemInfo;

                GetSystemInfo(&SystemInfo);

                if ((SystemInfo.dwProcessorType != PROCESSOR_INTEL_486) && 
                    (SystemInfo.dwProcessorType != PROCESSOR_INTEL_PENTIUM))
                    showMessage(ERR_GENERAL_WRONG_CPU);			
#endif
                /* Suppress file error messages from system like "Floppy A: not inserted" */
                SetErrorMode( SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS );

                /* initialize global mutex */
                g_Mutex = osl_createMutex();

                /* initialize "current directory" mutex */
                g_CurrentDirectoryMutex = osl_createMutex();

                /* request winsock rev. 1.1 */
                wVersionRequested = MAKEWORD(1, 1);
         
                error = WSAStartup(wVersionRequested, &wsaData);
                if ( 0 == error ) 
                {
                    WORD wMajorVersionRequired = 1;
                    WORD wMinorVersionRequired = 1;

                    if ((LOBYTE(wsaData.wVersion) <  wMajorVersionRequired) ||
                        (LOBYTE(wsaData.wVersion) == wMajorVersionRequired) &&
                        ((HIBYTE(wsaData.wVersion) < wMinorVersionRequired)))
                        {						
                            showMessage(ERR_WINSOCK_WRONG_VERSION);			
                        }
                }
                else
                {
                    showMessage(ERR_WINSOCK_INIT_FAILED);			
                }

                /* initialize Win9x unicode functions */				
                aInfo.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

                if ( GetVersionEx(&aInfo) )
                {
                    if ( VER_PLATFORM_WIN32_NT == aInfo.dwPlatformId )
                    {
                    }

                    g_dwPlatformId = aInfo.dwPlatformId;
                }				
                
                g_dwTLSTextEncodingIndex = TlsAlloc();
                InitializeCriticalSection( &g_ThreadKeyListCS );
        
                InitDCOM();

                //We disable floating point exceptions. This is the usual state at program startup
                //but on Windows 98 and ME this is not always the case.
                _control87(_MCW_EM, _MCW_EM);

                break;
            }

        case DLL_PROCESS_DETACH:

            WSACleanup( );

            TlsFree( g_dwTLSTextEncodingIndex );
            DeleteCriticalSection( &g_ThreadKeyListCS );

            osl_destroyMutex( g_Mutex );

            osl_destroyMutex( g_CurrentDirectoryMutex );

            /* finalize memory management */
            rtl_memory_fini();
            rtl_cache_fini();
            rtl_arena_fini();
            break;
    }

    return TRUE;
}

static DWORD GetParentProcessId()
{
    DWORD	dwParentProcessId = 0;
    HANDLE	hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

    if ( IsValidHandle( hSnapshot ) )
    {
        PROCESSENTRY32	pe;
        BOOL			fSuccess;

        ZeroMemory( &pe, sizeof(pe) );
        pe.dwSize = sizeof(pe);
        fSuccess = Process32First( hSnapshot, &pe );

        while( fSuccess )
        {
            if ( GetCurrentProcessId() == pe.th32ProcessID )
            {
                dwParentProcessId = pe.th32ParentProcessID;
                break;
            }

            fSuccess = Process32Next( hSnapshot, &pe );
        }

        CloseHandle( hSnapshot );
    }

    return dwParentProcessId;
}

static DWORD WINAPI ParentMonitorThreadProc( LPVOID lpParam )
{
    DWORD	dwParentProcessId = (DWORD)lpParam;

    HANDLE	hParentProcess = OpenProcess( SYNCHRONIZE, FALSE, dwParentProcessId );
    if ( IsValidHandle( hParentProcess ) )
    {
        if ( WAIT_OBJECT_0 == WaitForSingleObject( hParentProcess, INFINITE ) )
        {
            TerminateProcess( GetCurrentProcess(), 0 );
        }
        CloseHandle( hParentProcess );
    }
    return 0;
}

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved )
{
    (void)hinstDLL; /* avoid warning */
    (void)lpvReserved; /* avoid warning */
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            TCHAR	szBuffer[64];

            // This code will attach the process to it's parent process
            // if the parent process had set the environment variable.
            // The corresponding code (setting the environment variable)
            // is is desktop/win32/source/officeloader.cxx


            DWORD	dwResult = GetEnvironmentVariable( "ATTACHED_PARENT_PROCESSID", szBuffer, sizeof(szBuffer) );

            if ( dwResult && dwResult < sizeof(szBuffer) )
            {
                DWORD	dwThreadId = 0;

                DWORD	dwParentProcessId = (DWORD)atol( szBuffer );

                if ( dwParentProcessId && GetParentProcessId() == dwParentProcessId )
                {
                    // No error check, it works or it does not
                    CreateThread( NULL, 0, ParentMonitorThreadProc, (LPVOID)dwParentProcessId, 0, &dwThreadId );
                }
            }

            return TRUE;
        }

        case DLL_THREAD_ATTACH:			
            break;

        case DLL_THREAD_DETACH:
            _osl_callThreadKeyCallbackOnThreadDetach( );
            break;
    }

    return TRUE;
}

