/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: filelckb.cxx,v $
 * $Revision: 1.17 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_store.hxx"

#include <store/filelckb.hxx>

#ifndef INCLUDED_STDDEF_H
#include <stddef.h>
#define INCLUDED_STDDEF_H
#endif

#ifndef INCLUDED_STRING_H
#include <string.h>
#define INCLUDED_STRING_H
#endif
#include <sal/types.h>
#include <sal/macros.h>
#include <rtl/alloc.h>
#include <rtl/ustring.hxx>
#include <osl/file.h>
#include <osl/thread.h>
#include <osl/mutex.hxx>
#include <store/types.h>
#include <store/object.hxx>
#include <store/lockbyte.hxx>

using namespace store;

/*========================================================================
 *
 * OFileLockBytes internals.
 *
 *======================================================================*/

/*
 * __store_memcpy.
 */
inline void __store_memcpy (void * dst, const void * src, sal_uInt32 n)
{
    ::memcpy (dst, src, n);
}

/*
 * __store_errcode_mapping.
 */
struct __store_errcode_mapping_st
{
    sal_uInt32 m_nErrno;
    storeError m_nCode;
};

static storeError __store_errnoToErrCode (sal_uInt32 nErrno);
#define ERROR_FROM_NATIVE(e) __store_errnoToErrCode((sal_uInt32)(e))

/*
 * __store_file (basic file I/O; platform specific; inline).
 */
#define store_File_OpenRead     0x01L
#define store_File_OpenWrite    0x02L
#define store_File_OpenNoCreate 0x04L
#define store_File_OpenTruncate 0x08L

#if defined(SAL_OS2)
#include <fileos2.hxx>
#elif defined(SAL_UNX)
#include <fileunx.hxx>
#elif defined(SAL_W32)
#include <filew32.hxx>
#else  /* !(OS2 | UNX | W32) */
#include <filestd.hxx>
#endif /* !(OS2 | UNX | W32) */

/*
 * __store_errnoToErrCode.
 */
static storeError __store_errnoToErrCode (sal_uInt32 nErrno)
{
    int i, n = sizeof(__store_errcode_map) / sizeof(__store_errcode_map[0]);
    for (i = 0; i < n; i++)
    {
        if (__store_errcode_map[i].m_nErrno == nErrno)
            return __store_errcode_map[i].m_nCode;
    }
    return store_E_Unknown;
}

/*========================================================================
 *
 * FileMapping_Impl.
 *
 *======================================================================*/
namespace // unnamed
{

struct FileMapping_Impl
{
    /** Representation.
     */
    sal_uInt32 m_nAlignment;
    sal_uInt32 m_nSize;
    HSTORE     m_hMap;

    /** Construction.
     */
    FileMapping_Impl (void);
    ~FileMapping_Impl (void);

    /** Create readonly file mapping.
     */
    storeError create (HSTORE hFile);
};

/*
 * FileMapping_Impl.
 */
inline FileMapping_Impl::FileMapping_Impl (void)
    : m_nAlignment (__store_malign()), m_nSize (0), m_hMap (0)
{
}

/*
 * ~FileMapping_Impl.
 */
inline FileMapping_Impl::~FileMapping_Impl (void)
{
    if (m_hMap != 0)
        __store_funmap (m_hMap);
}

/*
 * create.
 */
inline storeError FileMapping_Impl::create (HSTORE hFile)
{
    if (m_nAlignment == (sal_uInt32)(-1))
        return store_E_Unknown; // E_Unsupported

    if ((m_hMap = __store_fmap (hFile)) == 0)
        return ERROR_FROM_NATIVE(__store_errno());

    return __store_fsize (hFile, m_nSize);
}

} // unnamed namespace

/*========================================================================
 *
 * MemoryMapping_Impl.
 *
 *======================================================================*/
namespace // unnamed
{

struct MemoryMapping_Impl
{
    /** Representation.
     */
    sal_uInt32  m_nOffset;
    sal_uInt32  m_nSize;
    sal_uInt8  *m_pData;

    /** Construction.
     */
    MemoryMapping_Impl (void);
    ~MemoryMapping_Impl (void);

    /** Check for a valid memory mapping.
     */
    bool isValid (void) const { return (m_pData != 0); }

    /** Create a readonly memory mapping.
     */
    storeError create (
        const FileMapping_Impl & fmap,
        sal_uInt32               nOffset,
        sal_uInt32               nSize);

    /** Cleanup (unmap) memory mapping.
     */
    void cleanup (void);
};

/*
 * MemoryMapping_Impl.
 */
inline MemoryMapping_Impl::MemoryMapping_Impl (void)
    : m_nOffset (0), m_nSize (0), m_pData (0)
{
}

/*
 * ~MemoryMapping_Impl.
 */
inline MemoryMapping_Impl::~MemoryMapping_Impl (void)
{
    if (m_pData != 0)
    {
        __store_munmap (m_pData, m_nSize);
        m_pData = 0, m_nSize = 0;
    }
}

/*
 * create.
 */
inline storeError MemoryMapping_Impl::create (
    const FileMapping_Impl & fmap, sal_uInt32 nOffset, sal_uInt32 nSize)
{
    storeError result = store_E_None;
    sal_uInt32 nAlign = (nOffset % fmap.m_nAlignment);

    nOffset -= nAlign;
    nSize   += nAlign;

    if ((nOffset + nSize) > fmap.m_nSize)
        nSize = fmap.m_nSize - nOffset;

    m_pData = __store_mmap (fmap.m_hMap, nOffset, nSize);
    if (m_pData == 0)
        result = ERROR_FROM_NATIVE(__store_errno());
    else
        m_nOffset = nOffset, m_nSize = nSize;
    return (result);
}

/*
 * cleanup.
 */
inline void MemoryMapping_Impl::cleanup (void)
{
    if (m_pData != 0)
    {
        __store_munmap (m_pData, m_nSize);
        m_pData = 0, m_nSize = 0;
    }
}

} // unnamed namespace

/*========================================================================
 *
 * OFileLockBytes_Impl interface.
 *
 *======================================================================*/
namespace store
{

class OFileLockBytes_Impl
{
    /** Representation.
     */
    HSTORE              m_hFile;
    MemoryMapping_Impl  m_aMemmap;
    bool                m_bWriteable;

public:
    /** Allocation.
     */
    static void * operator new (size_t n) SAL_THROW(())
    {
        return rtl_allocateMemory (sal_uInt32(n));
    }
    static void operator delete (void * p, size_t) SAL_THROW(())
    {
        rtl_freeMemory (p);
    }

    /** Construction.
     */
    OFileLockBytes_Impl (void);
    ~OFileLockBytes_Impl (void);

    /** Check for a valid file handle.
     */
    bool isValid (void) const { return (m_hFile != 0); }

    /** Operation.
     */
    storeError close (void);

    storeError create (
        const sal_Char *pszFilename,
        storeAccessMode eAccessMode);
    storeError create (
        rtl_uString    *pFilename,
        storeAccessMode eAccessMode);

    storeError resize (sal_uInt32 nSize);

    storeError readAt (
        sal_uInt32  nOffset,
        void       *pBuffer,
        sal_uInt32  nBytes,
        sal_uInt32 &rnDone);
    storeError writeAt (
        sal_uInt32  nOffset,
        const void *pBuffer,
        sal_uInt32  nBytes,
        sal_uInt32 &rnDone);

    storeError sync (void);
    storeError stat (sal_uInt32 &rnSize);
};

} // namespace store

/*========================================================================
 *
 * OFileLockBytes_Impl (inline) implementation.
 *
 *======================================================================*/
/*
 * OFileLockBytes_Impl.
 */
inline OFileLockBytes_Impl::OFileLockBytes_Impl (void)
    : m_hFile       (0),
      m_bWriteable  (false)
{
}

/*
 * ~OFileLockBytes_Impl.
 */
inline OFileLockBytes_Impl::~OFileLockBytes_Impl (void)
{
    m_aMemmap.cleanup();
    if (m_hFile)
    {
        (void)__store_fclose (m_hFile);
        m_hFile = 0;
    }
}

/*
 * close.
 */
inline storeError OFileLockBytes_Impl::close (void)
{
    m_aMemmap.cleanup();
    if (m_hFile)
    {
        (void)__store_fclose (m_hFile);
        m_hFile = 0;
    }
    return store_E_None;
}

/*
 * create(sal_Char*).
 */
inline storeError OFileLockBytes_Impl::create (
    const sal_Char *pszFilename, storeAccessMode eAccessMode)
{
    m_aMemmap.cleanup();
    if (m_hFile)
    {
        (void)__store_fclose (m_hFile);
        m_hFile = 0;
    }

    m_bWriteable = (!(eAccessMode == store_AccessReadOnly));

    sal_uInt32 nMode = store_File_OpenRead;
    if (m_bWriteable)
        nMode |= store_File_OpenWrite;
    else
        nMode |= store_File_OpenNoCreate;

    if (eAccessMode == store_AccessCreate)
        nMode |= store_File_OpenTruncate;
    if (eAccessMode == store_AccessReadWrite)
        nMode |= store_File_OpenNoCreate;

    storeError eErrCode = __store_fopen (pszFilename, nMode, m_hFile);
    if (eErrCode == store_E_None)
    {
        if (!m_bWriteable)
        {
            // Readonly, try Memory mapped I/O, ignore errors.
            FileMapping_Impl fmap;
            if (fmap.create (m_hFile) == store_E_None)
            {
                // Try to map the entire file into memory.
                m_aMemmap.create (fmap, 0, fmap.m_nSize);
            }
        }
    }
    return eErrCode;
}

/*
 * create(rtl_uString*).
 */
inline storeError OFileLockBytes_Impl::create (
    rtl_uString *pFilename, storeAccessMode eAccessMode)
{
    // Path conversion result.
    oslFileError result;

    // Convert into system path.
    rtl::OUString aSystemPath;
    result = osl_getSystemPathFromFileURL (pFilename, &(aSystemPath.pData));
    if (!(result == osl_File_E_None))
    {
        // Not FileUrl. Maybe a system path, already.
        result = osl_getFileURLFromSystemPath (
            pFilename, &(aSystemPath.pData));
        if (!(result == osl_File_E_None))
        {
            // Invalid path.
            return store_E_InvalidParameter;
        }

        rtl_uString_assign (&(aSystemPath.pData), pFilename);
    }

    // Convert into system text encoding.
    rtl::OString aFilename (
        aSystemPath.pData->buffer,
        aSystemPath.pData->length,
        osl_getThreadTextEncoding());

    // Open file.
    return create (aFilename.pData->buffer, eAccessMode);
}

/*
 * readAt.
 */
inline storeError OFileLockBytes_Impl::readAt (
    sal_uInt32  nOffset,
    void       *pBuffer,
    sal_uInt32  nBytes,
    sal_uInt32 &rnDone)
{
    if (m_aMemmap.isValid())
    {
        // Memory mapped I/O.
        if (!(nOffset < m_aMemmap.m_nSize))
            return store_E_None;

        nBytes = SAL_MIN(nOffset + nBytes, m_aMemmap.m_nSize) - nOffset;
        if (!(nBytes > 0))
            return store_E_None;

        __store_memcpy (pBuffer, m_aMemmap.m_pData + nOffset, nBytes);
        rnDone = nBytes;

        return store_E_None;
    }
    else
    {
        // File I/O.
        return __store_fread (m_hFile, nOffset, pBuffer, nBytes, rnDone);
    }
}

/*
 * writeAt.
 */
inline storeError OFileLockBytes_Impl::writeAt (
    sal_uInt32  nOffset,
    const void *pBuffer,
    sal_uInt32  nBytes,
    sal_uInt32 &rnDone)
{
    if (m_bWriteable)
        return __store_fwrite (m_hFile, nOffset, pBuffer, nBytes, rnDone);
    else
        return store_E_AccessViolation;
}

/*
 * resize.
 */
inline storeError OFileLockBytes_Impl::resize (sal_uInt32 nSize)
{
    if (m_bWriteable)
        return __store_ftrunc (m_hFile, nSize);
    else
        return store_E_AccessViolation;
}

/*
 * sync.
 */
inline storeError OFileLockBytes_Impl::sync (void)
{
    if (m_bWriteable)
        return __store_fsync (m_hFile);
    else
        return store_E_None;
}

/*
 * stat.
 */
inline storeError OFileLockBytes_Impl::stat (sal_uInt32 &rnSize)
{
    if (m_aMemmap.isValid())
    {
        // Memory mapped I/O.
        rnSize = m_aMemmap.m_nSize;
        return store_E_None;
    }
    return __store_fsize (m_hFile, rnSize);
}

/*========================================================================
 *
 * OFileLockBytes implementation.
 *
 *======================================================================*/
/*
 * OFileLockBytes.
 */
OFileLockBytes::OFileLockBytes (void)
    : m_pImpl (new OFileLockBytes_Impl())
{
}

/*
 * ~OFileLockBytes.
 */
OFileLockBytes::~OFileLockBytes (void)
{
    delete m_pImpl;
}

/*
 * acquire.
 */
oslInterlockedCount SAL_CALL OFileLockBytes::acquire (void)
{
    return OStoreObject::acquire();
}

/*
 * release.
 */
oslInterlockedCount SAL_CALL OFileLockBytes::release (void)
{
    return OStoreObject::release();
}

/*
 * create.
 */
storeError OFileLockBytes::create (
    rtl_uString     *pFilename,
    storeAccessMode  eAccessMode)
{
    // Acquire exclusive access.
    osl::MutexGuard aGuard (m_aMutex);

    if (pFilename)
        return m_pImpl->create (pFilename, eAccessMode);
    else
        return store_E_InvalidParameter;
}

/*
 * readAt.
 */
storeError OFileLockBytes::readAt (
    sal_uInt32  nOffset,
    void       *pBuffer,
    sal_uInt32  nBytes,
    sal_uInt32 &rnDone)
{
    // Initialize [out] param.
    rnDone = 0;

    // Acquire exclusive access.
    osl::MutexGuard aGuard (m_aMutex);
    if (m_pImpl->isValid())
        return m_pImpl->readAt (nOffset, pBuffer, nBytes, rnDone);
    else
        return store_E_InvalidHandle;
}

/*
 * writeAt.
 */
storeError OFileLockBytes::writeAt (
    sal_uInt32  nOffset,
    const void *pBuffer,
    sal_uInt32  nBytes,
    sal_uInt32 &rnDone)
{
    // Initialize [out] param.
    rnDone = 0;

    // Acquire exclusive access.
    osl::MutexGuard aGuard (m_aMutex);
    if (m_pImpl->isValid())
        return m_pImpl->writeAt (nOffset, pBuffer, nBytes, rnDone);
    else
        return store_E_InvalidHandle;
}

/*
 * flush.
 */
storeError OFileLockBytes::flush (void)
{
    // Acquire exclusive access.
    osl::MutexGuard aGuard (m_aMutex);
    if (m_pImpl->isValid())
        return m_pImpl->sync();
    else
        return store_E_InvalidHandle;
}

/*
 * setSize.
 */
storeError OFileLockBytes::setSize (sal_uInt32 nSize)
{
    // Acquire exclusive access.
    osl::MutexGuard aGuard (m_aMutex);
    if (m_pImpl->isValid())
        return m_pImpl->resize (nSize);
    else
        return store_E_InvalidHandle;
}

/*
 * stat.
 */
storeError OFileLockBytes::stat (sal_uInt32 &rnSize)
{
    // Initialize [out] param.
    rnSize = 0;

    // Acquire exclusive access.
    osl::MutexGuard aGuard (m_aMutex);
    if (m_pImpl->isValid())
        return m_pImpl->stat (rnSize);
    else
        return store_E_InvalidHandle;
}

/*
 * lockRange.
 */
storeError OFileLockBytes::lockRange (
    sal_uInt32 /* nOffset */, sal_uInt32 /* nBytes */)
{
    // Acquire exclusive access.
    osl::MutexGuard aGuard (m_aMutex);
    if (m_pImpl->isValid())
        return store_E_None; // E_Unsupported
    else
        return store_E_InvalidHandle;
}

/*
 * unlockRange.
 */
storeError OFileLockBytes::unlockRange (
    sal_uInt32 /* nOffset */, sal_uInt32 /* nBytes */)
{
    // Acquire exclusive access.
    osl::MutexGuard aGuard (m_aMutex);
    if (m_pImpl->isValid())
        return store_E_None; // E_Unsupported
    else
        return store_E_InvalidHandle;
}
