/*************************************************************************
 *
 *  $RCSfile: advapi9x.h,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: tra $ $Date: 2000-11-22 13:54:33 $
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

#ifdef __cplusplus
extern "C"{
#endif

//------------------------------------------------------------------------
// undefine the macros defined in the shlobj.h file in order to avoid
// warnings because of multiple defines 
//------------------------------------------------------------------------

// begin obsolete Win32 API functions --> 
#ifdef RegOpenKey
#undef RegOpenKey
#endif
#ifdef RegEnumKey
#undef RegEnumKey
#endif
#ifdef RegCreateKey
#undef RegCreateKey
#endif
#ifdef RegQueryValue
#undef RegQueryValue
#endif
#ifdef RegSetValue
#undef RegSetValue
#endif
// <-- end obsolete Win32 functions

#ifdef RegOpenKeyExW
#undef RegOpenKeyExW
#endif
#ifdef RegEnumKeyExW
#undef RegEnumKeyExW
#endif
#ifdef RegCreateKeyExW
#undef RegCreateKeyExW
#endif
#ifdef RegDeleteKeyW
#undef RegDeleteKeyW
#endif
#ifdef RegEnumValueW
#undef RegEnumValueW
#endif
#ifdef RegQueryValueExW
#undef RegQueryValueExW
#endif
#ifdef RegSetValueExW
#undef RegSetValueExW
#endif
#ifdef RegDeleteValueW
#undef RegDeleteValueW
#endif

//------------------------------------------------------------------------
// set the compiler directives for the function pointer we declare below
// if we build sal or sal will be used as static library we define extern
// else sal exports the function pointers from a dll and we use __declspec
//------------------------------------------------------------------------

#if defined(SAL_EXPORT_SYSTOOLS) || defined(USE_SAL_STATIC)
    #define ADVAPI9X_API extern
#else
    #define ADVAPI9X_API __declspec( dllimport )
#endif

//------------------------------------------------------------------------
// declare function pointers to the appropriate shell functions
//------------------------------------------------------------------------

ADVAPI9X_API LONG (WINAPI * lpfnRegOpenKeyExW) (
  HKEY hKey,         // handle to open key
  LPCWSTR lpSubKey,  // subkey name
  DWORD ulOptions,   // reserved
  REGSAM samDesired, // security access mask
  PHKEY phkResult    // handle to open key
);

ADVAPI9X_API LONG (WINAPI *lpfnRegEnumKeyExW) (
  HKEY hKey,                  // handle to key to enumerate
  DWORD dwIndex,              // subkey index
  LPWSTR lpName,              // subkey name
  LPDWORD lpcName,            // size of subkey buffer
  LPDWORD lpReserved,         // reserved
  LPWSTR lpClass,             // class string buffer
  LPDWORD lpcClass,           // size of class string buffer
  PFILETIME lpftLastWriteTime // last write time
);

ADVAPI9X_API LONG (WINAPI *lpfnRegCreateKeyExW)(
  HKEY hKey,                                  // handle to open key
  LPCWSTR lpSubKey,                           // subkey name
  DWORD Reserved,                             // reserved
  LPWSTR lpClass,                             // class string
  DWORD dwOptions,                            // special options
  REGSAM samDesired,                          // desired security access
  LPSECURITY_ATTRIBUTES lpSecurityAttributes, // inheritance
  PHKEY phkResult,                            // key handle 
  LPDWORD lpdwDisposition                     // disposition value buffer
);

ADVAPI9X_API LONG (WINAPI *lpfnRegDeleteKeyW) (
  HKEY hKey,         // handle to open key
  LPCWSTR lpSubKey   // subkey name
);

ADVAPI9X_API LONG (WINAPI *lpfnRegEnumValueW) (
  HKEY hKey,             // handle to key to query
  DWORD dwIndex,         // index of value to query
  LPWSTR lpValueName,    // value buffer
  LPDWORD lpcValueName,  // size of value buffer
  LPDWORD lpReserved,    // reserved
  LPDWORD lpType,        // type buffer
  LPBYTE lpData,         // data buffer
  LPDWORD lpcbData       // size of data buffer
);

ADVAPI9X_API LONG (WINAPI *lpfnRegQueryValueExW) (
  HKEY hKey,            // handle to key
  LPCWSTR lpValueName,  // value name
  LPDWORD lpReserved,   // reserved
  LPDWORD lpType,       // type buffer
  LPBYTE lpData,        // data buffer
  LPDWORD lpcbData      // size of data buffer
);

ADVAPI9X_API LONG (WINAPI *lpfnRegSetValueExW)(
  HKEY hKey,           // handle to key
  LPCWSTR lpValueName, // value name
  DWORD Reserved,      // reserved
  DWORD dwType,        // value type
  CONST BYTE *lpData,  // value data
  DWORD cbData         // size of value data
);

ADVAPI9X_API LONG (WINAPI *lpfnRegDeleteValueW) (
  HKEY hKey,            // handle to key
  LPCWSTR lpValueName   // value name
);

//------------------------------------------------------------------------
// the Advapi9xInit and Advapi9xDeInit functions will be used only by
// sal itself and will not be exported
//------------------------------------------------------------------------

#if defined(SAL_EXPORT_SYSTOOLS)
    extern void WINAPI Advapi9xInit( );
    extern void WINAPI Advapi9xDeInit( );
#endif

//------------------------------------------------------------------------
// redefine the above undefined macros so that the preprocessor replaces
// all occurrences of this macros with our function pointer
//------------------------------------------------------------------------

#define RegOpenKeyExW    lpfnRegOpenKeyExW
#define RegEnumKeyExW    lpfnRegEnumKeyExW
#define RegCreateKeyExW  lpfnRegCreateKeyExW
#define RegDeleteKeyW    lpfnRegDeleteKeyW
#define RegEnumValueW    lpfnRegEnumValueW
#define RegQueryValueExW lpfnRegQueryValueExW
#define RegSetValueExW   lpfnRegSetValueExW
#define RegDeleteValueW  lpfnRegDeleteValueW

#ifdef __cplusplus
}
#endif
