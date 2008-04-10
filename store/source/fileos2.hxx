/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: fileos2.hxx,v $
 * $Revision: 1.6 $
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

#ifndef INCLUDED_STORE_FILEOS2_HXX
#define INCLUDED_STORE_FILEOS2_HXX

#define INCL_DOS
#define INCL_DOSERRORS
#include <os2.h>

typedef HFILE HSTORE;

/*========================================================================
 *
 * File I/O (inline) implementation.
 *
 *======================================================================*/
/*
 * __store_errcode_map.
 */
static const __store_errcode_mapping_st __store_errcode_map[] =
{
    { NO_ERROR,                   store_E_None             },
    { ERROR_FILE_NOT_FOUND,       store_E_NotExists        },
    { ERROR_PATH_NOT_FOUND,       store_E_NotExists        },
    { ERROR_ACCESS_DENIED,        store_E_AccessViolation  },
    { ERROR_SHARING_VIOLATION,    store_E_AccessViolation  },
    { ERROR_LOCK_VIOLATION,       store_E_LockingViolation },
    { ERROR_INVALID_ACCESS,       store_E_InvalidAccess    },
    { ERROR_INVALID_HANDLE,       store_E_InvalidHandle    },
    { ERROR_INVALID_PARAMETER,    store_E_InvalidParameter },
    { ERROR_FILENAME_EXCED_RANGE, store_E_NameTooLong      },
    { ERROR_TOO_MANY_OPEN_FILES,  store_E_NoMoreFiles      }
};

/*
 * __store_errno.
 */
inline sal_Int32 __store_errno (void)
{
    return (sal_Int32)errno;
}

/*
 * __store_malign (unsupported).
 */
inline sal_uInt32 __store_malign (void)
{
    return (sal_uInt32)(-1);
}

/*
 * __store_fmap (readonly, unsupported).
 */
inline HSTORE __store_fmap (HSTORE hFile)
{
    return ((HSTORE)0);
}

/*
 * __store_funmap.
 */
inline void __store_funmap (HSTORE hMap)
{
}

/*
 * __store_mmap (readonly, unsupported).
 */
inline sal_uInt8* __store_mmap (HSTORE h, sal_uInt32 k, sal_uInt32 n)
{
    return (sal_uInt8*)NULL;
}

/*
 * __store_munmap (unsupported).
 */
inline void __store_munmap (sal_uInt8 *p, sal_uInt32 n)
{
}

/*
 * __store_fopen.
 */
inline storeError __store_fopen (
    const sal_Char *pszName, sal_uInt32 nMode, HSTORE &rhFile)
{
    // Initialize [out] param.
    rhFile = 0;

    // Access mode.
    sal_uInt32 nAccessMode = OPEN_ACCESS_READONLY;
    if (nMode & store_File_OpenWrite)
        nAccessMode = OPEN_ACCESS_READWRITE;

    if (nAccessMode == OPEN_ACCESS_READONLY)
    {
        nMode |=  store_File_OpenNoCreate;
        nMode &= ~store_File_OpenTruncate;
    }

    // Share mode.
    sal_uInt32 nShareMode = OPEN_SHARE_DENYNONE;
    if (nMode & store_File_OpenWrite)
        nShareMode = OPEN_SHARE_DENYWRITE;

    // Open action.
    sal_uInt32 nOpenAction = 0, nDoneAction = 0;
    if (!(nMode & store_File_OpenNoCreate))
        nOpenAction = OPEN_ACTION_CREATE_IF_NEW; // Open always.
    else
        nOpenAction = OPEN_ACTION_FAIL_IF_NEW;   // Open existing.

    if (nMode & store_File_OpenTruncate)
        nOpenAction |= OPEN_ACTION_REPLACE_IF_EXISTS;
    else
        nOpenAction |= OPEN_ACTION_OPEN_IF_EXISTS;

    // Create file handle.
    APIRET result = ::DosOpen (
        (PSZ)pszName,
        &rhFile,
        &nDoneAction,
        0L,
        FILE_NORMAL,
        nOpenAction,
        nAccessMode | nShareMode | OPEN_FLAGS_NOINHERIT,
        0L);

    // Check result.
    if (result)
        return ERROR_FROM_NATIVE(result);
    else
        return store_E_None;
}

/*
 * __store_fread.
 */
inline storeError __store_fread (
    HSTORE h, sal_uInt32 offset, void *p, sal_uInt32 n, sal_uInt32 &k)
{
    APIRET result;
    if ((result = ::DosSetFilePtr (h, (long)offset, FILE_BEGIN, &k)) != 0)
        return ERROR_FROM_NATIVE(result);
    if ((result = ::DosRead (h, p, n, &k)) != 0)
        return ERROR_FROM_NATIVE(result);
    else
        return store_E_None;
}

/*
 * __store_fwrite.
 */
inline storeError __store_fwrite (
    HSTORE h, sal_uInt32 offset, const void *p, sal_uInt32 n, sal_uInt32 &k)
{
    APIRET result;
    if ((result = ::DosSetFilePtr (h, (long)offset, FILE_BEGIN, &k)) != 0)
        return ERROR_FROM_NATIVE(result);
    if ((result = ::DosWrite (h, (PVOID)p, n, &k)) != 0)
        return ERROR_FROM_NATIVE(result);
    else
        return store_E_None;
}

/*
 * __store_fseek.
 */
inline storeError __store_fseek (HSTORE h, sal_uInt32 n)
{
    sal_uInt32 k = 0;
    APIRET result = ::DosSetFilePtr (h, (long)n, FILE_BEGIN, &k);
    if (result)
        return ERROR_FROM_NATIVE(result);
    else
        return store_E_None;
}

/*
 * __store_fsize.
 */
inline storeError __store_fsize (HSTORE h, sal_uInt32 &k)
{
    APIRET result = ::DosSetFilePtr (h, 0L, FILE_END, &k);
    if (result)
        return ERROR_FROM_NATIVE(result);
    else
        return store_E_None;
}

/*
 * __store_ftrunc.
 */
inline storeError __store_ftrunc (HSTORE h, sal_uInt32 n)
{
    APIRET result = ::DosSetFileSize (h, n);
    if (result)
        return ERROR_FROM_NATIVE(result);
    else
        return store_E_None;
}

/*
 * __store_fsync.
 */
inline storeError __store_fsync (HSTORE h)
{
    APIRET result = ::DosResetBuffer (h);
    if (result)
        return ERROR_FROM_NATIVE(result);
    else
        return store_E_None;
}

/*
 * __store_fclose.
 */
inline storeError __store_fclose (HSTORE h)
{
    APIRET result = ::DosClose (h);
    if (result)
        return ERROR_FROM_NATIVE(result);
    else
        return store_E_None;
}

#endif /* INCLUDED_STORE_FILEOS2_HXX */
