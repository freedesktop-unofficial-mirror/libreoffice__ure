/*************************************************************************
 *
 *  $RCSfile: CommandLineToArgvW.cpp,v $
 *
 *  $Revision: 1.3 $
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

#include "macros.h"

#ifdef __cplusplus
#define local inline
#else
#define local static
#endif

local LPCWSTR SkipBlanks( LPCWSTR lpScan )
{
    while ( ' ' == *lpScan || '\t' == *lpScan ) 
        lpScan++;

    return lpScan;
}


local LPCWSTR SkipArgument( LPCWSTR lpScan )
{
    BOOL	fQuoted = FALSE;
    LPCWSTR	lpArgEnd = NULL;

    do
    {
        switch ( *lpScan )
        {
        case ' ':
        case '\t':
            if ( fQuoted )
                lpScan++;
            else
                lpArgEnd = lpScan;
            break;
        case '\"':
            lpScan++;
            fQuoted = !fQuoted;
            break;
        case '\0':
            lpArgEnd = lpScan;
            break;
        default:
            lpScan++;
            break;
        }
    } while( *lpScan && !lpArgEnd );

    return lpScan;
}

    
IMPLEMENT_THUNK( shell32, WINDOWS, LPWSTR *, WINAPI, CommandLineToArgvW, ( LPCWSTR lpCmdLineW, int *pNumArgs ) )
{
    LPWSTR	*lpArgvW = NULL;

    if ( !lpCmdLineW || !*lpCmdLineW )
    {
        CHAR	szFileName[MAX_PATH];

        DWORD	dwResult = GetModuleFileNameA( NULL, szFileName, MAX_PATH );

        if ( dwResult && dwResult < MAX_PATH )
        {
            int cchNeeded = MultiByteToWideChar( CP_ACP, 0, szFileName, -1, NULL, 0 );

            lpArgvW = (LPWSTR *)GlobalAlloc( 0, cchNeeded * sizeof(WCHAR) + sizeof(LPWSTR) );

            if ( lpArgvW )
            {
                lpArgvW[0] = (LPWSTR)(lpArgvW + 1);

                MultiByteToWideChar( CP_ACP, 0, szFileName, -1, lpArgvW[0], cchNeeded );
                *pNumArgs = 1;
            }
            else
                SetLastError( ERROR_OUTOFMEMORY );
        }
    }
    else
    {
        LPCWSTR	lpScan = lpCmdLineW;
        int		nTokens = 0;
        int		cchNeeded = 0;
        
        // Count arguments and required size

        while ( *lpScan )
        {
            lpScan = SkipBlanks( lpScan );
            if ( *lpScan )
            {
                LPCWSTR	lpArgEnd = SkipArgument( lpScan );

                nTokens++;
                cchNeeded += lpArgEnd - lpScan + 1;
                lpScan = lpArgEnd;
            }
        }

        // Allocate space for one additional NULL pointer to terminate list
        
        lpArgvW = (LPWSTR *)GlobalAlloc( 0, sizeof(LPWSTR) * (nTokens + 1) + sizeof(WCHAR) * cchNeeded );

        if ( lpArgvW )
        {
            // Collect arguments

            LPWSTR	lpDestination = (LPWSTR)&lpArgvW[nTokens + 1];

            lpScan = lpCmdLineW;
            nTokens = 0;

            while ( *lpScan )
            {
                lpScan = SkipBlanks( lpScan );
                if ( *lpScan )
                {
                    LPCWSTR	lpArgEnd = SkipArgument( lpScan );

                    lpArgvW[nTokens++] = lpDestination;

                    while ( lpScan < lpArgEnd )
                    {
                        if ( '\"' != *lpScan )
                            *lpDestination++ = *lpScan;

                        lpScan++;
                    }
                    *lpDestination++ = 0;
                }
            }

            lpArgvW[nTokens] = NULL;

            *pNumArgs = nTokens;
        }
        else
            SetLastError( ERROR_OUTOFMEMORY );
        
    }

    return lpArgvW;
}
