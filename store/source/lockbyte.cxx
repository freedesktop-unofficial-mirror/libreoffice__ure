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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_store.hxx"

#include "lockbyte.hxx"

#include "sal/types.h"
#include "osl/diagnose.h"
#include "osl/file.h"
#include "osl/process.h"
#include "rtl/alloc.h"
#include "rtl/ustring.hxx"

#include "object.hxx"
#include "storbase.hxx"

#ifndef INCLUDED_STRING_H
#include <string.h>
#define INCLUDED_STRING_H
#endif

using namespace store;

/*========================================================================
 *
 * ILockBytes (non-virtual interface) implementation.
 *
 *======================================================================*/

storeError ILockBytes::initialize (rtl::Reference< PageData::Allocator > & rxAllocator, sal_uInt16 nPageSize)
{
    OSL_PRECOND((STORE_MINIMUM_PAGESIZE <= nPageSize) && (nPageSize <= STORE_MAXIMUM_PAGESIZE), "invalid PageSize");
    return initialize_Impl (rxAllocator, nPageSize);
}

storeError ILockBytes::readPageAt (PageHolder & rPage, sal_uInt32 nOffset)
{
    OSL_PRECOND(!(nOffset == STORE_PAGE_NULL), "store::ILockBytes::readPageAt(): invalid Offset");
    if (nOffset == STORE_PAGE_NULL)
        return store_E_CantSeek;

    return readPageAt_Impl (rPage, nOffset);
}

storeError ILockBytes::writePageAt (PageHolder const & rPage, sal_uInt32 nOffset)
{
    // [SECURITY:ValInput]
    PageData const * pagedata = rPage.get();
    OSL_PRECOND(!(pagedata == 0), "store::ILockBytes::writePageAt(): invalid Page");
    if (pagedata == 0)
        return store_E_InvalidParameter;

    sal_uInt32 const offset = pagedata->location();
    OSL_PRECOND(!(nOffset != offset), "store::ILockBytes::writePageAt(): inconsistent Offset");
    if (nOffset != offset)
        return store_E_InvalidParameter;

    OSL_PRECOND(!(nOffset == STORE_PAGE_NULL), "store::ILockBytes::writePageAt(): invalid Offset");
    if (nOffset == STORE_PAGE_NULL)
        return store_E_CantSeek;

    return writePageAt_Impl (rPage, nOffset);
}

storeError ILockBytes::readAt (sal_uInt32 nOffset, void * pBuffer, sal_uInt32 nBytes)
{
    // [SECURITY:ValInput]
    sal_uInt8 * dst_lo = static_cast<sal_uInt8*>(pBuffer);
    if (!(dst_lo != 0))
        return store_E_InvalidParameter;

    sal_uInt8 * dst_hi = dst_lo + nBytes;
    if (!(dst_lo < dst_hi))
        return (dst_lo > dst_hi) ? store_E_InvalidParameter : store_E_None;

    OSL_PRECOND(!(nOffset == STORE_PAGE_NULL), "store::ILockBytes::readAt(): invalid Offset");
    if (nOffset == STORE_PAGE_NULL)
        return store_E_CantSeek;

    sal_uInt64 const src_size = nOffset + nBytes;
    if (src_size > SAL_MAX_UINT32)
        return store_E_CantSeek;

    return readAt_Impl (nOffset, dst_lo, (dst_hi - dst_lo));
}

storeError ILockBytes::writeAt (sal_uInt32 nOffset, void const * pBuffer, sal_uInt32 nBytes)
{
    // [SECURITY:ValInput]
    sal_uInt8 const * src_lo = static_cast<sal_uInt8 const*>(pBuffer);
    if (!(src_lo != 0))
        return store_E_InvalidParameter;

    sal_uInt8 const * src_hi = src_lo + nBytes;
    if (!(src_lo < src_hi))
        return (src_lo > src_hi) ? store_E_InvalidParameter : store_E_None;

    OSL_PRECOND(!(nOffset == STORE_PAGE_NULL), "store::ILockBytes::writeAt(): invalid Offset");
    if (nOffset == STORE_PAGE_NULL)
        return store_E_CantSeek;

    sal_uInt64 const dst_size = nOffset + nBytes;
    if (dst_size > SAL_MAX_UINT32)
        return store_E_CantSeek;

    return writeAt_Impl (nOffset, src_lo, (src_hi - src_lo));
}

storeError ILockBytes::getSize (sal_uInt32 & rnSize)
{
    rnSize = 0;
    return getSize_Impl (rnSize);
}

storeError ILockBytes::setSize (sal_uInt32 nSize)
{
    return setSize_Impl (nSize);
}

storeError ILockBytes::flush()
{
    return flush_Impl();
}

/*========================================================================
 *
 * FileLockBytes implementation.
 *
 *======================================================================*/
namespace store
{

struct FileHandle
{
    oslFileHandle m_handle;

    FileHandle() : m_handle(0) {}

    bool operator != (FileHandle const & rhs)
    {
        return (m_handle != rhs.m_handle);
    }

    static storeError errorFromNative (oslFileError eErrno)
    {
        switch (eErrno)
        {
        case osl_File_E_None:
            return store_E_None;

        case osl_File_E_NOENT:
            return store_E_NotExists;

        case osl_File_E_ACCES:
        case osl_File_E_PERM:
            return store_E_AccessViolation;

        case osl_File_E_AGAIN:
        case osl_File_E_DEADLK:
            return store_E_LockingViolation;

        case osl_File_E_BADF:
            return store_E_InvalidHandle;

        case osl_File_E_INVAL:
            return store_E_InvalidParameter;

        case osl_File_E_NOMEM:
            return store_E_OutOfMemory;

        case osl_File_E_NOSPC:
            return store_E_OutOfSpace;

        case osl_File_E_OVERFLOW:
            return store_E_CantSeek;

        default:
            return store_E_Unknown;
        }
    }

    static sal_uInt32 modeToNative (storeAccessMode eAccessMode)
    {
        sal_uInt32 nFlags = 0;
        switch (eAccessMode)
        {
        case store_AccessCreate:
        case store_AccessReadCreate:
            nFlags |= osl_File_OpenFlag_Create;
            // fall through
        case store_AccessReadWrite:
            nFlags |= osl_File_OpenFlag_Write;
            // fall through
        case store_AccessReadOnly:
            nFlags |= osl_File_OpenFlag_Read;
            break;
        default:
            OSL_PRECOND(0, "store::FileHandle: unknown storeAccessMode");
        }
        return nFlags;
    }

    storeError initialize (rtl_uString * pFilename, storeAccessMode eAccessMode)
    {
        // Verify arguments.
        sal_uInt32 nFlags = modeToNative (eAccessMode);
        if (!pFilename || !nFlags)
            return store_E_InvalidParameter;

        // Convert into FileUrl.
        rtl::OUString aFileUrl;
        if (osl_getFileURLFromSystemPath (pFilename, &(aFileUrl.pData)) != osl_File_E_None)
        {
            // Not system path. Assume file url.
            rtl_uString_assign (&(aFileUrl.pData), pFilename);
        }
        if (aFileUrl.compareToAscii("file://", 7) != 0)
        {
            // Not file url. Assume relative path.
            rtl::OUString aCwdUrl;
            (void) osl_getProcessWorkingDir (&(aCwdUrl.pData));

            // Absolute file url.
            (void) osl_getAbsoluteFileURL (aCwdUrl.pData, aFileUrl.pData, &(aFileUrl.pData));
        }

        // Acquire handle.
        oslFileError result = osl_openFile (aFileUrl.pData, &m_handle, nFlags);
        if (result == osl_File_E_EXIST)
        {
            // Already existing (O_CREAT | O_EXCL).
            result = osl_openFile (aFileUrl.pData, &m_handle, osl_File_OpenFlag_Read | osl_File_OpenFlag_Write);
            if ((result == osl_File_E_None) && (eAccessMode == store_AccessCreate))
            {
                // Truncate existing file.
                result = osl_setFileSize (m_handle, 0);
            }
        }
        if (result != osl_File_E_None)
            return errorFromNative(result);
        return store_E_None;
    }

    /** @see FileLockBytes destructor
     */
    static void closeFile (oslFileHandle hFile)
    {
        (void) osl_closeFile (hFile);
    }

    /** @see ResourceHolder<T>::destructor_type
     */
    struct CloseFile
    {
        void operator()(FileHandle & rFile) const
        {
            // Release handle.
            closeFile (rFile.m_handle);
            rFile.m_handle = 0;
        }
    };
    typedef CloseFile destructor_type;
};

class FileLockBytes :
    public store::OStoreObject,
    public store::ILockBytes
{
    /** Representation.
     */
    oslFileHandle                         m_hFile;
    sal_uInt32                            m_nSize;
    rtl::Reference< PageData::Allocator > m_xAllocator;

    storeError initSize_Impl (sal_uInt32 & rnSize);

    /** ILockBytes implementation.
     */
    virtual storeError initialize_Impl (rtl::Reference< PageData::Allocator > & rxAllocator, sal_uInt16 nPageSize);

    virtual storeError readPageAt_Impl (PageHolder & rPage, sal_uInt32 nOffset);
    virtual storeError writePageAt_Impl (PageHolder const & rPage, sal_uInt32 nOffset);

    virtual storeError readAt_Impl (sal_uInt32 nOffset, void * pBuffer, sal_uInt32 nBytes);
    virtual storeError writeAt_Impl (sal_uInt32 nOffset, void const * pBuffer, sal_uInt32 nBytes);

    virtual storeError getSize_Impl (sal_uInt32 & rnSize);
    virtual storeError setSize_Impl (sal_uInt32 nSize);

    virtual storeError flush_Impl();

    /** Not implemented.
     */
    FileLockBytes (FileLockBytes const &);
    FileLockBytes & operator= (FileLockBytes const &);

public:
    /** Construction.
     */
    explicit FileLockBytes (FileHandle & rFile);

    /** Delegate multiple inherited IReference.
     */
    virtual oslInterlockedCount SAL_CALL acquire();
    virtual oslInterlockedCount SAL_CALL release();

protected:
    /** Destruction.
     */
    virtual ~FileLockBytes();
};

} // namespace store

FileLockBytes::FileLockBytes (FileHandle & rFile)
    : m_hFile (rFile.m_handle), m_nSize (SAL_MAX_UINT32), m_xAllocator()
{
}

FileLockBytes::~FileLockBytes()
{
    FileHandle::closeFile (m_hFile);
}

oslInterlockedCount SAL_CALL FileLockBytes::acquire()
{
    return OStoreObject::acquire();
}

oslInterlockedCount SAL_CALL FileLockBytes::release()
{
    return OStoreObject::release();
}

storeError FileLockBytes::initSize_Impl (sal_uInt32 & rnSize)
{
    /* osl_getFileSize() uses slow 'fstat(h, &size)',
     * instead of fast 'size = lseek(h, 0, SEEK_END)'.
     * so, init size here, and track changes.
     */
    sal_uInt64 uSize = 0;
    oslFileError result = osl_getFileSize (m_hFile, &uSize);
    if (result != osl_File_E_None)
        return FileHandle::errorFromNative(result);
    if (uSize > SAL_MAX_UINT32)
        return store_E_CantSeek;

    rnSize = sal::static_int_cast<sal_uInt32>(uSize);
    return store_E_None;
}

storeError FileLockBytes::initialize_Impl (rtl::Reference< PageData::Allocator > & rxAllocator, sal_uInt16 nPageSize)
{
    storeError result = initSize_Impl (m_nSize);
    if (result != store_E_None)
        return (result);

    result = PageData::Allocator::createInstance (rxAllocator, nPageSize);
    if (result != store_E_None)
        return (result);

    // @see readPageAt_Impl().
    m_xAllocator = rxAllocator;
    return store_E_None;
}

storeError FileLockBytes::readPageAt_Impl (PageHolder & rPage, sal_uInt32 nOffset)
{
    if (m_xAllocator.is())
    {
        PageHolder page (m_xAllocator->construct<PageData>(), m_xAllocator);
        page.swap (rPage);
    }

    if (!m_xAllocator.is())
        return store_E_InvalidAccess;
    if (!rPage.get())
        return store_E_OutOfMemory;

    PageData * pagedata = rPage.get();
    return readAt_Impl (nOffset, pagedata, pagedata->size());
}

storeError FileLockBytes::writePageAt_Impl (PageHolder const & rPage, sal_uInt32 nOffset)
{
    PageData const * pagedata = rPage.get();
    OSL_PRECOND(pagedata != 0, "contract violation");
    return writeAt_Impl (nOffset, pagedata, pagedata->size());
}

storeError FileLockBytes::readAt_Impl (sal_uInt32 nOffset, void * pBuffer, sal_uInt32 nBytes)
{
    sal_uInt64 nDone = 0;
    oslFileError result = osl_readFileAt (m_hFile, nOffset, pBuffer, nBytes, &nDone);
    if (result != osl_File_E_None)
        return FileHandle::errorFromNative(result);
    if (nDone != nBytes)
        return (nDone != 0) ? store_E_CantRead : store_E_NotExists;
    return store_E_None;
}

storeError FileLockBytes::writeAt_Impl (sal_uInt32 nOffset, void const * pBuffer, sal_uInt32 nBytes)
{
    sal_uInt64 nDone = 0;
    oslFileError result = osl_writeFileAt (m_hFile, nOffset, pBuffer, nBytes, &nDone);
    if (result != osl_File_E_None)
        return FileHandle::errorFromNative(result);
    if (nDone != nBytes)
        return store_E_CantWrite;

    sal_uInt64 const uSize = nOffset + nBytes;
    OSL_PRECOND(uSize < SAL_MAX_UINT32, "store::ILockBytes::writeAt() contract violation");
    if (uSize > m_nSize)
        m_nSize = sal::static_int_cast<sal_uInt32>(uSize);
    return store_E_None;
}

storeError FileLockBytes::getSize_Impl (sal_uInt32 & rnSize)
{
    rnSize = m_nSize;
    return store_E_None;
}

storeError FileLockBytes::setSize_Impl (sal_uInt32 nSize)
{
    oslFileError result = osl_setFileSize (m_hFile, nSize);
    if (result != osl_File_E_None)
        return FileHandle::errorFromNative(result);

    m_nSize = nSize;
    return store_E_None;
}

storeError FileLockBytes::flush_Impl()
{
    oslFileError result = osl_syncFile (m_hFile);
    if (result != osl_File_E_None)
        return FileHandle::errorFromNative(result);
    return store_E_None;
}

/*========================================================================
 *
 * MappedLockBytes implementation.
 *
 *======================================================================*/
namespace store
{

struct FileMapping
{
    sal_uInt8 * m_pAddr;
    sal_uInt32  m_nSize;

    FileMapping() : m_pAddr(0), m_nSize(0) {}

    bool operator != (FileMapping const & rhs) const
    {
        return ((m_pAddr != rhs.m_pAddr) || (m_nSize != rhs.m_nSize));
    }

    oslFileError initialize (oslFileHandle hFile)
    {
        // Determine mapping size.
        sal_uInt64   uSize  = 0;
        oslFileError result = osl_getFileSize (hFile, &uSize);
        if (result != osl_File_E_None)
            return result;

        // [SECURITY:IntOver]
        if (uSize > SAL_MAX_UINT32)
            return osl_File_E_OVERFLOW;
        m_nSize = sal::static_int_cast<sal_uInt32>(uSize);

        // Acquire mapping.
        return osl_mapFile (hFile, reinterpret_cast<void**>(&m_pAddr), m_nSize, 0, osl_File_MapFlag_RandomAccess);
    }

    /** @see MappedLockBytes::destructor.
     */
    static void unmapFile (sal_uInt8 * pAddr, sal_uInt32 nSize)
    {
        (void) osl_unmapFile (pAddr, nSize);
    }

    /** @see ResourceHolder<T>::destructor_type
     */
    struct UnmapFile
    {
        void operator ()(FileMapping & rMapping) const
        {
            // Release mapping.
            unmapFile (rMapping.m_pAddr, rMapping.m_nSize);
            rMapping.m_pAddr = 0, rMapping.m_nSize = 0;
        }
    };
    typedef UnmapFile destructor_type;
};

class MappedLockBytes :
    public store::OStoreObject,
    public store::PageData::Allocator,
    public store::ILockBytes
{
    /** Representation.
     */
    sal_uInt8 * m_pData;
    sal_uInt32  m_nSize;
    sal_uInt16  m_nPageSize;

    /** PageData::Allocator implementation.
     */
    virtual void allocate_Impl (void ** ppPage, sal_uInt16 * pnSize);
    virtual void deallocate_Impl (void * pPage);

    /** ILockBytes implementation.
     */
    virtual storeError initialize_Impl (rtl::Reference< PageData::Allocator > & rxAllocator, sal_uInt16 nPageSize);

    virtual storeError readPageAt_Impl (PageHolder & rPage, sal_uInt32 nOffset);
    virtual storeError writePageAt_Impl (PageHolder const & rPage, sal_uInt32 nOffset);

    virtual storeError readAt_Impl  (sal_uInt32 nOffset, void * pBuffer, sal_uInt32 nBytes);
    virtual storeError writeAt_Impl (sal_uInt32 nOffset, const void * pBuffer, sal_uInt32 nBytes);

    virtual storeError getSize_Impl (sal_uInt32 & rnSize);
    virtual storeError setSize_Impl (sal_uInt32 nSize);

    virtual storeError flush_Impl();

    /** Not implemented.
     */
    MappedLockBytes (MappedLockBytes const &);
    MappedLockBytes & operator= (MappedLockBytes const &);

public:
    /** Construction.
     */
    explicit MappedLockBytes (FileMapping & rMapping);

    /** Delegate multiple inherited IReference.
     */
    virtual oslInterlockedCount SAL_CALL acquire();
    virtual oslInterlockedCount SAL_CALL release();

protected:
    /* Destruction.
     */
    virtual ~MappedLockBytes();
};

} // namespace store

MappedLockBytes::MappedLockBytes (FileMapping & rMapping)
    : m_pData (rMapping.m_pAddr), m_nSize (rMapping.m_nSize), m_nPageSize(0)
{
}

MappedLockBytes::~MappedLockBytes()
{
    FileMapping::unmapFile (m_pData, m_nSize);
}

oslInterlockedCount SAL_CALL MappedLockBytes::acquire()
{
    return OStoreObject::acquire();
}

oslInterlockedCount SAL_CALL MappedLockBytes::release()
{
    return OStoreObject::release();
}

void MappedLockBytes::allocate_Impl (void ** ppPage, sal_uInt16 * pnSize)
{
    OSL_PRECOND((ppPage != 0) && (pnSize != 0), "contract violation");
    if ((ppPage != 0) && (pnSize != 0))
        *ppPage = 0, *pnSize = m_nPageSize;
}

void MappedLockBytes::deallocate_Impl (void * pPage)
{
    OSL_PRECOND((m_pData <= pPage) && (pPage < m_pData + m_nSize), "contract violation");
    (void)pPage; // UNUSED
}

storeError MappedLockBytes::initialize_Impl (rtl::Reference< PageData::Allocator > & rxAllocator, sal_uInt16 nPageSize)
{
    rxAllocator = this;
    m_nPageSize = nPageSize;
    return store_E_None;
}

storeError MappedLockBytes::readPageAt_Impl (PageHolder & rPage, sal_uInt32 nOffset)
{
    sal_uInt8 * src_lo = m_pData + nOffset;
    if ((m_pData > src_lo) || (src_lo >= m_pData + m_nSize))
        return store_E_NotExists;

    sal_uInt8 * src_hi = src_lo + m_nPageSize;
    if ((m_pData > src_hi) || (src_hi > m_pData + m_nSize))
        return store_E_CantRead;

    PageHolder page (reinterpret_cast< PageData* >(src_lo), static_cast< PageData::Allocator* >(this));
    page.swap (rPage);

    return store_E_None;
}

storeError MappedLockBytes::writePageAt_Impl (PageHolder const & /*rPage*/, sal_uInt32 /*nOffset*/)
{
    return store_E_AccessViolation;
}

storeError MappedLockBytes::readAt_Impl (sal_uInt32 nOffset, void * pBuffer, sal_uInt32 nBytes)
{
    sal_uInt8 const * src_lo = m_pData + nOffset;
    if ((m_pData > src_lo) || (src_lo >= m_pData + m_nSize))
        return store_E_NotExists;

    sal_uInt8 const * src_hi = src_lo + nBytes;
    if ((m_pData > src_hi) || (src_hi > m_pData + m_nSize))
        return store_E_CantRead;

    memcpy (pBuffer, src_lo, (src_hi - src_lo));
    return store_E_None;
}

storeError MappedLockBytes::writeAt_Impl (sal_uInt32 /*nOffset*/, void const * /*pBuffer*/, sal_uInt32 /*nBytes*/)
{
    return store_E_AccessViolation;
}

storeError MappedLockBytes::getSize_Impl (sal_uInt32 & rnSize)
{
    rnSize = m_nSize;
    return store_E_None;
}

storeError MappedLockBytes::setSize_Impl (sal_uInt32 /*nSize*/)
{
    return store_E_AccessViolation;
}

storeError MappedLockBytes::flush_Impl()
{
    return store_E_None;
}

/*========================================================================
 *
 * MemoryLockBytes implementation.
 *
 *======================================================================*/
namespace store
{

class MemoryLockBytes :
    public store::OStoreObject,
    public store::ILockBytes
{
    /** Representation.
     */
    sal_uInt8 * m_pData;
    sal_uInt32  m_nSize;
    rtl::Reference< PageData::Allocator > m_xAllocator;

    /** ILockBytes implementation.
     */
    virtual storeError initialize_Impl (rtl::Reference< PageData::Allocator > & rxAllocator, sal_uInt16 nPageSize);

    virtual storeError readPageAt_Impl (PageHolder & rPage, sal_uInt32 nOffset);
    virtual storeError writePageAt_Impl (PageHolder const & rPage, sal_uInt32 nOffset);

    virtual storeError readAt_Impl  (sal_uInt32 nOffset, void * pBuffer, sal_uInt32 nBytes);
    virtual storeError writeAt_Impl (sal_uInt32 nOffset, const void * pBuffer, sal_uInt32 nBytes);

    virtual storeError getSize_Impl (sal_uInt32 & rnSize);
    virtual storeError setSize_Impl (sal_uInt32 nSize);

    virtual storeError flush_Impl();

    /** Not implemented.
     */
    MemoryLockBytes (MemoryLockBytes const &);
    MemoryLockBytes& operator= (MemoryLockBytes const &);

public:
    /** Construction.
     */
    MemoryLockBytes();

    /** Delegate multiple inherited IReference.
     */
    virtual oslInterlockedCount SAL_CALL acquire();
    virtual oslInterlockedCount SAL_CALL release();

protected:
    /** Destruction.
     */
    virtual ~MemoryLockBytes();
};

} // namespace store

MemoryLockBytes::MemoryLockBytes()
    : m_pData (0), m_nSize (0), m_xAllocator()
{}

MemoryLockBytes::~MemoryLockBytes()
{
    rtl_freeMemory (m_pData);
}

oslInterlockedCount SAL_CALL MemoryLockBytes::acquire (void)
{
    return OStoreObject::acquire();
}

oslInterlockedCount SAL_CALL MemoryLockBytes::release (void)
{
    return OStoreObject::release();
}

storeError MemoryLockBytes::initialize_Impl (rtl::Reference< PageData::Allocator > & rxAllocator, sal_uInt16 nPageSize)
{
    storeError result = PageData::Allocator::createInstance (rxAllocator, nPageSize);
    if (result == store_E_None)
    {
        // @see readPageAt_Impl().
        m_xAllocator = rxAllocator;
    }
    return result;
}

storeError MemoryLockBytes::readPageAt_Impl (PageHolder & rPage, sal_uInt32 nOffset)
{
    if (m_xAllocator.is())
    {
        PageHolder page (m_xAllocator->construct<PageData>(), m_xAllocator);
        page.swap (rPage);
    }

    if (!m_xAllocator.is())
        return store_E_InvalidAccess;
    if (!rPage.get())
        return store_E_OutOfMemory;

    PageData * pagedata = rPage.get();
    return readAt_Impl (nOffset, pagedata, pagedata->size());
}

storeError MemoryLockBytes::writePageAt_Impl (PageHolder const & rPage, sal_uInt32 nOffset)
{
    PageData const * pagedata = rPage.get();
    OSL_PRECOND(!(pagedata == 0), "contract violation");
    return writeAt_Impl (nOffset, pagedata, pagedata->size());
}

storeError MemoryLockBytes::readAt_Impl (sal_uInt32 nOffset, void * pBuffer, sal_uInt32 nBytes)
{
    sal_uInt8 const * src_lo = m_pData + nOffset;
    if ((m_pData > src_lo) || (src_lo >= m_pData + m_nSize))
        return store_E_NotExists;

    sal_uInt8 const * src_hi = src_lo + nBytes;
    if ((m_pData > src_hi) || (src_hi > m_pData + m_nSize))
        return store_E_CantRead;

    memcpy (pBuffer, src_lo, (src_hi - src_lo));
    return store_E_None;
}

storeError MemoryLockBytes::writeAt_Impl (sal_uInt32 nOffset, const void * pBuffer, sal_uInt32 nBytes)
{
    sal_uInt64 const dst_size = nOffset + nBytes;
    OSL_PRECOND(dst_size < SAL_MAX_UINT32, "store::ILockBytes::writeAt() contract violation");
    if (dst_size > m_nSize)
    {
        storeError eErrCode = setSize_Impl (sal::static_int_cast<sal_uInt32>(dst_size));
        if (eErrCode != store_E_None)
            return eErrCode;
    }
    OSL_POSTCOND(dst_size <= m_nSize, "store::MemoryLockBytes::setSize_Impl() contract violation");

    sal_uInt8 * dst_lo = m_pData + nOffset;
    if (dst_lo >= m_pData + m_nSize)
        return store_E_CantSeek;

    sal_uInt8 * dst_hi = dst_lo + nBytes;
    if (dst_hi > m_pData + m_nSize)
        return store_E_CantWrite;

    memcpy (dst_lo, pBuffer, (dst_hi - dst_lo));
    return store_E_None;
}

storeError MemoryLockBytes::getSize_Impl (sal_uInt32 & rnSize)
{
    rnSize = m_nSize;
    return store_E_None;
}

storeError MemoryLockBytes::setSize_Impl (sal_uInt32 nSize)
{
    if (nSize != m_nSize)
    {
        sal_uInt8 * pData = reinterpret_cast<sal_uInt8*>(rtl_reallocateMemory (m_pData, nSize));
        if (pData != 0)
        {
            if (nSize > m_nSize)
                memset (pData + m_nSize, 0, sal::static_int_cast<size_t>(nSize - m_nSize));
        }
        else
        {
            if (nSize != 0)
                return store_E_OutOfMemory;
        }
        m_pData = pData, m_nSize = nSize;
    }
    return store_E_None;
}

storeError MemoryLockBytes::flush_Impl()
{
    return store_E_None;
}

/*========================================================================
 *
 * ILockBytes factory implementations.
 *
 *======================================================================*/
namespace store
{

template< class T > struct ResourceHolder
{
    typedef typename T::destructor_type destructor_type;

    T m_value;

    explicit ResourceHolder (T const & value = T()) : m_value (value) {}
    ~ResourceHolder() { reset(); }

    T & get() { return m_value; }
    T const & get() const { return m_value; }

    void set (T const & value) { m_value = value; }
    void reset (T const & value = T())
    {
        T tmp (m_value);
        if (tmp != value)
            destructor_type()(tmp);
        set (value);
    }
    T release()
    {
        T tmp (m_value);
        set (T());
        return tmp;
    }

    ResourceHolder (ResourceHolder & rhs)
    {
        set (rhs.release());
    }
    ResourceHolder & operator= (ResourceHolder & rhs)
    {
        reset (rhs.release());
        return *this;
    }
};

storeError
FileLockBytes_createInstance (
    rtl::Reference< ILockBytes > & rxLockBytes,
    rtl_uString *                  pFilename,
    storeAccessMode                eAccessMode
)
{
    // Acquire file handle.
    ResourceHolder<FileHandle> xFile;
    storeError result = xFile.get().initialize (pFilename, eAccessMode);
    if (result != store_E_None)
        return (result);

    if (eAccessMode == store_AccessReadOnly)
    {
        ResourceHolder<FileMapping> xMapping;
        if (xMapping.get().initialize (xFile.get().m_handle) == osl_File_E_None)
        {
            rxLockBytes = new MappedLockBytes (xMapping.get());
            if (!rxLockBytes.is())
                return store_E_OutOfMemory;
            (void) xMapping.release();
        }
    }
    if (!rxLockBytes.is())
    {
        rxLockBytes = new FileLockBytes (xFile.get());
        if (!rxLockBytes.is())
            return store_E_OutOfMemory;
        (void) xFile.release();
    }

    return store_E_None;
}

storeError
MemoryLockBytes_createInstance (
    rtl::Reference< ILockBytes > & rxLockBytes
)
{
    rxLockBytes = new MemoryLockBytes();
    if (!rxLockBytes.is())
        return store_E_OutOfMemory;

    return store_E_None;
}

} // namespace store

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
