/*************************************************************************
 *
 *  $RCSfile: shell9x.h,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: hr $ $Date: 2000-09-18 15:17:15 $
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
#pragma once 

#ifndef _WINDOWS_
#include <windows.h>
#endif

#ifndef _SHLOBJ_H_
#include <shlobj.h>
#endif

#ifdef __cplusplus
extern "C"{
#endif

/* undefine if already a macro */
#ifdef CommandLineToArgvW
#undef CommandLineToArgvW
#endif

#ifdef SHBrowseForFolderW
#undef SHBrowseForFolderW
#endif

#ifdef SHGetPathFromIDListW
#undef SHGetPathFromIDListW
#endif

#ifdef SetWindowTextW
#undef SetWindowTextW
#endif

extern LPWSTR * (WINAPI *lpfnCommandLineToArgvW) (
  LPCWSTR lpCmdLine,  // pointer to a command-line string
  int *pNumArgs       // receives the argument count
);

extern LPITEMIDLIST ( WINAPI * lpfnSHBrowseForFolderW) (
    LPBROWSEINFOW lpbi
);

extern BOOL ( WINAPI * lpfnSHGetPathFromIDListW ) (
    LPCITEMIDLIST pidl, 
    LPWSTR pszPath
);

extern BOOL ( WINAPI * lpfnSetWindowTextW ) (
    HWND hWnd,
    LPCWSTR lpString
);

/* define as macro */
#define CommandLineToArgvW   lpfnCommandLineToArgvW
#define SHBrowseForFolderW   lpfnSHBrowseForFolderW
#define SHGetPathFromIDListW lpfnSHGetPathFromIDListW
#define SetWindowTextW       lpfnSetWindowTextW

void Shell9xInit(LPOSVERSIONINFO lpVersionInfo);
void Shell9xDeInit();

#ifdef __cplusplus
}
#endif
