/*************************************************************************
 *
 *  $RCSfile: DllGetVersion.cpp,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hro $ $Date: 2002-08-26 13:43:19 $
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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>
#include <malloc.h>

extern HMODULE		UWINAPI_BaseAddress;

// This function should be exported by every DLL that wants to provide it's version number.
// This code automaticly generates the information from the version resource 

extern "C" HRESULT CALLBACK DllGetVersion( DLLVERSIONINFO *pdvi )
{
    TCHAR	szModulePath[MAX_PATH];
    BOOL	fSuccess = FALSE;

    if ( UWINAPI_BaseAddress && GetModuleFileName( UWINAPI_BaseAddress, szModulePath, MAX_PATH ) )
    {
        DWORD	dwHandle = 0;
        DWORD	dwSize = GetFileVersionInfoSize( szModulePath, &dwHandle );
        LPVOID	lpData = _alloca( dwSize );
        
        if ( GetFileVersionInfo( szModulePath, dwHandle, dwSize, lpData ) )
        {
            VS_FIXEDFILEINFO	*lpBuffer = NULL;
            UINT	uLen = 0;

            if ( VerQueryValue( lpData, TEXT("\\"), (LPVOID *)&lpBuffer, &uLen ) )
            {
                pdvi->dwMajorVersion = HIWORD( lpBuffer->dwFileVersionMS );
                pdvi->dwMinorVersion = LOWORD( lpBuffer->dwFileVersionMS );
                pdvi->dwBuildNumber = HIWORD( lpBuffer->dwFileVersionLS );
                pdvi->dwPlatformID = (lpBuffer->dwFileOS & VOS_NT) ? DLLVER_PLATFORM_NT : DLLVER_PLATFORM_WINDOWS;

                fSuccess = TRUE;
            }
        }
    }

    return fSuccess ? HRESULT_FROM_WIN32( GetLastError() ) : HRESULT_FROM_WIN32( NO_ERROR );
}

