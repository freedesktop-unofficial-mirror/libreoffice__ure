/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: FindFirstVolumeMountPointW.cpp,v $
 * $Revision: 1.5.38.2 $
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
#include "macros.h"
#ifdef __MINGW32__
DEFINE_DEFAULT_THUNK( kernel32, TRYLOAD, HANDLE, WINAPI, FindFirstVolumeMountPointW, (LPWSTR lpszRootPathName, LPWSTR lpszVolumeMountPoint, DWORD cchBufferLength) )
#else
DEFINE_DEFAULT_THUNK( kernel32, TRYLOAD, HANDLE, WINAPI, FindFirstVolumeMountPointW, (LPCWSTR lpszRootPathName, LPWSTR lpszVolumeMountPoint, DWORD cchBufferLength) )
#endif
