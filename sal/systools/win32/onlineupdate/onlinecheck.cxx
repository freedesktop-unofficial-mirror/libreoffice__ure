/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: onlinecheck.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: ihi $ $Date: 2006-08-04 10:07:36 $
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
#pragma warning( disable: 4668 )

#include <sal/types.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wininet.h>

#ifdef UNICODE
#define _UNICODE
#endif
#include <tchar.h>

#define elementsof(a) (sizeof(a)/sizeof((a)[0]))

sal_Bool SAL_CALL hasInternetConnection()
{
    DWORD	dwFlags;
    TCHAR	szConnectionName[1024];

    BOOL fIsConnected = InternetGetConnectedStateEx(
        &dwFlags,
        szConnectionName,
        elementsof(szConnectionName),
        0 );

    return fIsConnected ? sal_True : sal_False;
}
