/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: GetUserDomainW.cpp,v $
 * $Revision: 1.4 $
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

EXTERN_C DWORD WINAPI GetUserDomainW_NT( LPWSTR lpBuffer, DWORD nSize )
#include "GetUserDomain_NT.cpp"


EXTERN_C DWORD WINAPI GetUserDomainW_WINDOWS( LPWSTR lpBuffer, DWORD nSize )
#include "GetUserDomain_WINDOWS.cpp"

EXTERN_C void WINAPI ResolveThunk_GetUserDomainW( FARPROC *lppfn, LPCSTR lpLibFileName, LPCSTR lpFuncName )
{
    if ( (LONG)GetVersion() < 0 )
        *lppfn = (FARPROC)GetUserDomainW_WINDOWS;
    else
        *lppfn = (FARPROC)GetUserDomainW_NT;
}

DEFINE_CUSTOM_THUNK( kernel32, GetUserDomainW, DWORD, WINAPI, GetUserDomainW, ( LPWSTR lpBuffer, DWORD cchBuffer ) );

