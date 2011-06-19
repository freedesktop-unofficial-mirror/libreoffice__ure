/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#define UNICODE
#include "macros.h"

EXTERN_C DWORD WINAPI GetLongPathNameW_NT( LPCWSTR lpShortPath, LPWSTR lpLongPath, DWORD cchBuffer )
#include "GetLongPathName.cpp"

EXTERN_C DWORD WINAPI GetLongPathNameW_WINDOWS( LPCWSTR lpShortPathW, LPWSTR lpLongPathW, DWORD cchBuffer )
{
    AUTO_WSTR2STR( lpShortPath );
    AUTO_STR( lpLongPath, cchBuffer );

    DWORD	dwResult = GetLongPathNameA( lpShortPathA, lpLongPathA, cchBuffer );

    if ( dwResult && dwResult < cchBuffer )
        STR2WSTR( lpLongPath, cchBuffer );

    return dwResult;
}


EXTERN_C void WINAPI ResolveThunk_GetLongPathNameW( FARPROC *lppfn, LPCSTR lpLibFileName, LPCSTR lpFuncName )
{
    if ( (LONG)GetVersion() < 0 )
        *lppfn = (FARPROC)GetLongPathNameW_WINDOWS;
    else
    {
        FARPROC	lpfnResult = GetProcAddress( LoadLibraryA( lpLibFileName ), lpFuncName );
        if ( !lpfnResult )
            lpfnResult = (FARPROC)GetLongPathNameW_NT;

        *lppfn = lpfnResult;
    }
}


DEFINE_CUSTOM_THUNK( kernel32, GetLongPathNameW, DWORD, WINAPI, GetLongPathNameW, ( LPCWSTR lpShortPathW, LPWSTR lpLongPathW, DWORD cchBuffer ) );

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
