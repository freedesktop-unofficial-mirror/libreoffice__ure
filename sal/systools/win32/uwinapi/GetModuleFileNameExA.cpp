/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#include "macros.h"
#ifdef _MSC_VER
#pragma warning(push,1) // disable warnings within system headers
#endif
#include <psapi.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include <tlhelp32.h>

IMPLEMENT_THUNK( psapi, WINDOWS, DWORD, WINAPI, GetModuleFileNameExA, (HANDLE hProcess, HMODULE hModule, LPSTR lpFileName, DWORD nSize ) )
{
    DWORD	dwProcessId = 0;
    DWORD	dwResult = 0;

    if ( !hProcess || hProcess == GetCurrentProcess() || GetCurrentProcessId() == (dwProcessId = GetProcessId( hProcess )) )
        return GetModuleFileNameA( hModule, lpFileName, nSize );

    HANDLE	hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwProcessId );

    if ( IsValidHandle( hSnapshot ) )
    {
        MODULEENTRY32	me;

        me.dwSize = sizeof(me);
        if ( Module32First( hSnapshot, &me ) )
        {
            BOOL	fFound = FALSE;

            if ( NULL == hModule )
                fFound = TRUE;
            else do
            {
                fFound = (me.hModule == hModule);
            } while ( !fFound && Module32Next( hSnapshot, &me ) );
            
            if ( fFound )
            {
                dwResult = _tcslen( me.szExePath );

                if ( dwResult > nSize && nSize > 0 )
                    lpFileName[nSize -1] = 0;

                _tcsncpy( lpFileName, me.szExePath, nSize );
            }
        }

        CloseHandle( hSnapshot );
    }
    
    return dwResult;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
