/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: storbase.hxx,v $
 * $Revision: 1.10 $
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

#ifndef _STORE_STORBASE_HXX_
#define _STORE_STORBASE_HXX_ "$Revision: 1.10 $"

#include <sal/types.h>
#include <rtl/alloc.h>
#include <rtl/string.h>
#include <rtl/ref.hxx>
#include <osl/endian.h>
#include <osl/mutex.hxx>
#include <store/types.h>
#include <store/object.hxx>
#include <store/lockbyte.hxx>

#ifndef INCLUDED_CSTDDEF
#include <cstddef>
#define INCLUDED_CSTDDEF
#endif

#ifndef INCLUDED_CSTRING
#include <cstring>
#define INCLUDED_CSTRING
#endif

/*========================================================================
 *
 * store common internals.
 *
 *======================================================================*/
/* MSVC 6.0 still has std functions in global namespace */
#if defined(_MSC_VER) && (_MSC_VER <= 1310)
#define __STORE_CSTD
#else
#define __STORE_CSTD std
#endif /* _MSC_VER */

#ifndef __STORE_DELETEZ
#define __STORE_DELETEZ(p) (delete p, p = 0)
#endif

/*
 * __store_memcpy.
 */
inline void __store_memcpy (void * dst, const void * src, sal_Size n)
{
    __STORE_CSTD::memcpy (dst, src, n);
}

/*
 * __store_memmove.
 */
inline void __store_memmove (void * dst, const void * src, sal_Size n)
{
    __STORE_CSTD::memmove (dst, src, n);
}

/*
 * __store_memset.
 */
inline void __store_memset (void * dst, int val, sal_Size n)
{
    __STORE_CSTD::memset (dst, val, n);
}

namespace store
{

/*========================================================================
 *
 * OStorePageGuard.
 *
 *======================================================================*/
struct OStorePageGuard
{
    /** Representation.
     */
    sal_uInt32 m_nMagic;
    sal_uInt32 m_nCRC32;

    /** Construction.
     */
    OStorePageGuard (sal_uInt32 nMagic = 0, sal_uInt32 nCRC32 = 0)
        : m_nMagic (nMagic),
          m_nCRC32 (nCRC32)
    {}

    OStorePageGuard (const OStorePageGuard& rOther)
        : m_nMagic (rOther.m_nMagic),
          m_nCRC32 (rOther.m_nCRC32)
    {}

    OStorePageGuard& operator= (const OStorePageGuard& rOther)
    {
        m_nMagic = rOther.m_nMagic;
        m_nCRC32 = rOther.m_nCRC32;
        return *this;
    }

    /** Comparison.
     */
    sal_Bool operator== (const OStorePageGuard& rOther) const
    {
        return ((m_nMagic == rOther.m_nMagic) &&
                (m_nCRC32 == rOther.m_nCRC32)    );
    }

    /** swap (internal and external representation).
     */
    void swap (void)
    {
#ifdef OSL_BIGENDIAN
        m_nMagic = OSL_SWAPDWORD(m_nMagic);
        m_nCRC32 = OSL_SWAPDWORD(m_nCRC32);
#endif /* OSL_BIGENDIAN */
    }

    /** CRC polynomial 0xEDB88320.
     */
    static const sal_uInt32 m_pTable[256];

    static sal_uInt32 updcrc32 (sal_uInt32 crc, sal_uInt8 octet)
    {
        return m_pTable[((crc) ^ (octet)) & 0xff] ^ ((crc) >> 8);
    }

    static sal_uInt32 crc32 (
        sal_uInt32 nCRC32, const void *pData, sal_Size nSize);
};
    
/*========================================================================
 *
 * OStorePageDescriptor.
 *
 *======================================================================*/
#define STORE_PAGE_NULL ((sal_uInt32)(~0))

struct OStorePageDescriptor
{
    /** Representation.
     */
    sal_uInt32 m_nAddr;
    sal_uInt16 m_nSize;
    sal_uInt16 m_nUsed;

    /** Construction.
     */
    OStorePageDescriptor (
        sal_uInt32 nAddr = STORE_PAGE_NULL,
        sal_uInt16 nSize = 0,
        sal_uInt16 nUsed = 0)
        : m_nAddr (nAddr),
          m_nSize (nSize),
          m_nUsed (nUsed)
    {}

    OStorePageDescriptor (const OStorePageDescriptor& rOther)
        : m_nAddr (rOther.m_nAddr),
          m_nSize (rOther.m_nSize),
          m_nUsed (rOther.m_nUsed)
    {}

    OStorePageDescriptor& operator= (const OStorePageDescriptor& rOther)
    {
        m_nAddr = rOther.m_nAddr;
        m_nSize = rOther.m_nSize;
        m_nUsed = rOther.m_nUsed;
        return *this;
    }

    /** Comparison.
     */
    sal_Bool operator== (const OStorePageDescriptor& rOther) const
    {
        return ((m_nAddr == rOther.m_nAddr) &&
                (m_nSize == rOther.m_nSize)    );
    }

    sal_Bool operator<= (const OStorePageDescriptor& rOther) const
    {
        return ((m_nAddr == rOther.m_nAddr) &&
                (m_nSize <= rOther.m_nSize)    );
    }

    sal_Bool operator< (const OStorePageDescriptor& rOther) const
    {
        if (m_nAddr == rOther.m_nAddr)
            return (m_nSize < rOther.m_nSize);
        else
            return (m_nAddr < rOther.m_nAddr);
    }

    /** swap (internal and external representation).
     */
    void swap (void)
    {
#ifdef OSL_BIGENDIAN
        m_nAddr = OSL_SWAPDWORD(m_nAddr);
        m_nSize = OSL_SWAPWORD(m_nSize);
        m_nUsed = OSL_SWAPWORD(m_nUsed);
#endif /* OSL_BIGENDIAN */
    }
};
    
/*========================================================================
 *
 * OStorePageKey.
 *
 *======================================================================*/
struct OStorePageKey
{
    /** Representation.
     */
    sal_uInt32 m_nLow;
    sal_uInt32 m_nHigh;

    /** Construction.
     */
    OStorePageKey (sal_uInt32 nLow = 0, sal_uInt32 nHigh = 0)
        : m_nLow (nLow), m_nHigh (nHigh)
    {}

    OStorePageKey (const OStorePageKey& rOther)
        : m_nLow (rOther.m_nLow), m_nHigh (rOther.m_nHigh)
    {}

    OStorePageKey& operator= (const OStorePageKey& rOther)
    {
        m_nLow  = rOther.m_nLow;
        m_nHigh = rOther.m_nHigh;
        return *this;
    }

    /** Comparison.
     */
    sal_Bool operator== (const OStorePageKey& rOther) const
    {
        return ((m_nLow  == rOther.m_nLow ) &&
                (m_nHigh == rOther.m_nHigh)    );
    }

    sal_Bool operator< (const OStorePageKey& rOther) const
    {
        if (m_nHigh == rOther.m_nHigh)
            return (m_nLow < rOther.m_nLow);
        else
            return (m_nHigh < rOther.m_nHigh);
    }

    /** swap (internal and external representation).
     */
    void swap (void)
    {
#ifdef OSL_BIGENDIAN
        m_nLow  = OSL_SWAPDWORD(m_nLow);
        m_nHigh = OSL_SWAPDWORD(m_nHigh);
#endif /* OSL_BIGENDIAN */
    }
};

/*========================================================================
 *
 * OStorePageLink.
 *
 *======================================================================*/
struct OStorePageLink
{
    /** Representation.
     */
    sal_uInt32 m_nAddr;

    /** Construction.
     */
    OStorePageLink (sal_uInt32 nAddr = STORE_PAGE_NULL)
        : m_nAddr (nAddr)
    {}

    OStorePageLink (const OStorePageLink& rOther)
        : m_nAddr (rOther.m_nAddr)
    {}

    OStorePageLink& operator= (const OStorePageLink& rOther)
    {
        m_nAddr = rOther.m_nAddr;
        return *this;
    }

    /** Comparison.
     */
    sal_Bool operator== (const OStorePageLink& rOther) const
    {
        return (m_nAddr == rOther.m_nAddr);
    }

    sal_Bool operator< (const OStorePageLink& rOther) const
    {
        return (m_nAddr < rOther.m_nAddr);
    }

    /** Operation.
     */
    void link (OStorePageLink& rPred)
    {
        OStorePageLink tmp (rPred);
        rPred = *this;
        *this = tmp;
    }

    void unlink (OStorePageLink& rPred)
    {
        rPred = *this;
        *this = OStorePageLink();
    }

    /** swap (internal and external representation).
     */
    void swap (void)
    {
#ifdef OSL_BIGENDIAN
        m_nAddr = OSL_SWAPDWORD(m_nAddr);
#endif /* OSL_BIGENDIAN */
    }
};

/*========================================================================
 *
 * OStorePageNameBlock.
 *
 *======================================================================*/
struct OStorePageNameBlock
{
    typedef OStorePageGuard G;
    typedef OStorePageKey   K;

    /** Representation.
    */
    G          m_aGuard;
    K          m_aKey;
    sal_uInt32 m_nAttrib;
    sal_Char   m_pData[STORE_MAXIMUM_NAMESIZE];

    /** size.
    */
    static sal_uInt16 size (void)
    {
        return sal_uInt16(sizeof(G) + sizeof(K) + sizeof(sal_uInt32) +
                          sizeof(sal_Char[STORE_MAXIMUM_NAMESIZE]));
    }

    /** initialize.
    */
    void initialize (void)
    {
        m_aGuard  = G();
        m_aKey    = K();
        m_nAttrib = 0;
        __store_memset (m_pData, 0, sizeof(m_pData));
    }

    /** Construction.
    */
    OStorePageNameBlock (void)
        : m_nAttrib (0)
    {
        __store_memset (m_pData, 0, sizeof(m_pData));
    }

    /** Comparison.
    */
    sal_Bool operator== (const OStorePageNameBlock& rOther) const
    {
        return (m_aGuard == rOther.m_aGuard);
    }

    /** swap (internal and external representation).
    */
    void swap (void)
    {
#ifdef OSL_BIGENDIAN
        m_aGuard.swap();
        m_aKey.swap();
        m_nAttrib = OSL_SWAPDWORD(m_nAttrib);
#endif /* OSL_BIGENDIAN */
    }

    /** guard (external representation).
    */
    void guard (void)
    {
        sal_uInt32 nCRC32 = 0;
        nCRC32 = G::crc32 (nCRC32, &m_aGuard.m_nMagic, sizeof(sal_uInt32));
        nCRC32 = G::crc32 (nCRC32, &m_aKey, size() - sizeof(G));
#ifdef OSL_BIGENDIAN
        nCRC32 = OSL_SWAPDWORD(nCRC32);
#endif /* OSL_BIGENDIAN */
        m_aGuard.m_nCRC32 = nCRC32;
    }

    /** verify (external representation).
    */
    storeError verify (void)
    {
        sal_uInt32 nCRC32 = 0;
        nCRC32 = G::crc32 (nCRC32, &m_aGuard.m_nMagic, sizeof(sal_uInt32));
        nCRC32 = G::crc32 (nCRC32, &m_aKey, size() - sizeof(G));
#ifdef OSL_BIGENDIAN
        nCRC32 = OSL_SWAPDWORD(nCRC32);
#endif /* OSL_BIGENDIAN */
        if (m_aGuard.m_nCRC32 != nCRC32)
            return store_E_InvalidChecksum;
        else
            return store_E_None;
    }

    /** namei.
    */
    static storeError namei (
        const rtl_String *pPath, const rtl_String *pName, K &rKey)
    {
        // Check parameter.
        if (!(pPath && pName))
            return store_E_InvalidParameter;

        // Check name length.
        if (!(pName->length < STORE_MAXIMUM_NAMESIZE))
            return store_E_NameTooLong;

        // Transform pathname into key.
        rKey.m_nLow  = G::crc32 (0, pName->buffer, pName->length);
        rKey.m_nHigh = G::crc32 (0, pPath->buffer, pPath->length);

        // Done.
        return store_E_None;
    }
};

/*========================================================================
 *
 * OStorePageData.
 *
 *======================================================================*/
struct OStorePageData
{
    typedef OStorePageGuard      G;
    typedef OStorePageDescriptor D;
    typedef OStorePageLink       L;

    /** Representation.
     */
    G m_aGuard;
    D m_aDescr;
    L m_aMarked;
    L m_aUnused;

    /** size.
     */
    static sal_uInt16 size (void)
    {
        return sal_uInt16(sizeof(G) + sizeof(D) + 2 * sizeof(L));
    }

    /** location.
     */
    sal_uInt32 location (void) const
    {
        return m_aDescr.m_nAddr;
    }

    void location (sal_uInt32 nAddr)
    {
        m_aDescr.m_nAddr = nAddr;
    }

    /** Allocation.
     */
    static void* operator new (size_t n)
    {
        return rtl_allocateMemory (n);
    }

    static void* operator new (size_t, sal_uInt16 nPageSize)
    {
        return rtl_allocateMemory (nPageSize);
    }

    static void operator delete (void *p)
    {
        rtl_freeMemory (p);
    }

    static void operator delete (void *p, sal_uInt16)
    {
        rtl_freeMemory (p);
    }

    /** Construction.
     */
    OStorePageData (sal_uInt16 nPageSize)
    {
        m_aDescr.m_nSize = nPageSize;
        m_aDescr.m_nUsed = size();
    }

    OStorePageData& operator= (const OStorePageData& rOther)
    {
        m_aGuard  = rOther.m_aGuard;
        m_aDescr  = rOther.m_aDescr;
        m_aMarked = rOther.m_aMarked;
        m_aUnused = rOther.m_aUnused;
        return *this;
    }

    /** Comparison.
     */
    sal_Bool operator== (const OStorePageData& rOther) const
    {
        return ((m_aGuard  == rOther.m_aGuard ) &&
                (m_aDescr  == rOther.m_aDescr ) &&
                (m_aMarked == rOther.m_aMarked) &&
                (m_aUnused == rOther.m_aUnused)    );
    }

    /** swap (internal and external representation).
     */
    void swap ()
    {
#ifdef OSL_BIGENDIAN
        m_aGuard.swap();
        m_aDescr.swap();
        m_aMarked.swap();
        m_aUnused.swap();
#endif /* OSL_BIGENDIAN */
    }

    /** guard (external representation).
     */
    void guard ()
    {
        sal_uInt32 nCRC32 = 0;
        nCRC32 = G::crc32 (nCRC32, &m_aGuard.m_nMagic, sizeof(sal_uInt32));
        nCRC32 = G::crc32 (nCRC32, &m_aDescr, size() - sizeof(G));
#ifdef OSL_BIGENDIAN
        nCRC32 = OSL_SWAPDWORD(nCRC32);
#endif /* OSL_BIGENDIAN */
        m_aGuard.m_nCRC32 = nCRC32;
    }

    /** verify (external representation).
     */
    storeError verify ()
    {
        sal_uInt32 nCRC32 = 0;
        nCRC32 = G::crc32 (nCRC32, &m_aGuard.m_nMagic, sizeof(sal_uInt32));
        nCRC32 = G::crc32 (nCRC32, &m_aDescr, size() - sizeof(G));
#ifdef OSL_BIGENDIAN
        nCRC32 = OSL_SWAPDWORD(nCRC32);
#endif /* OSL_BIGENDIAN */
        if (m_aGuard.m_nCRC32 != nCRC32)
            return store_E_InvalidChecksum;
        else
            return store_E_None;
    }
};

/*========================================================================
 *
 * OStorePageObject.
 *
 *======================================================================*/
class OStorePageObject
{
    typedef OStorePageData       page;
    typedef OStorePageDescriptor D;

public:
    /** Allocation.
     */
    static void * operator new (std::size_t n) SAL_THROW(())
    {
        return rtl_allocateMemory (sal_uInt32(n));
    }
    static void operator delete (void * p, std::size_t) SAL_THROW(())
    {
        rtl_freeMemory (p);
    }

    /** Construction.
     */
    inline OStorePageObject (page& rPage);

    /** Destruction.
     */
    virtual ~OStorePageObject (void);

    /** External representation.
     */
    virtual void       swap   (const D& rDescr);
    virtual void       guard  (const D& rDescr);
    virtual storeError verify (const D& rDescr);

    /** Data.
     */
    inline OStorePageData& getData (void);

    /** State.
     */
    inline sal_Bool dirty (void) const;
    inline void     clean (void);
    inline void     touch (void);

    /** Location.
     */
    inline sal_uInt32 location (void) const;
    inline void       location (sal_uInt32 nAddr);

private:
    /** Representation.
     */
    page     &m_rPage;
    sal_Bool  m_bDirty;
};

inline OStorePageObject::OStorePageObject (page& rPage)
    : m_rPage (rPage), m_bDirty (sal_False)
{
}

inline OStorePageData& OStorePageObject::getData (void)
{
    return m_rPage;
}

inline sal_Bool OStorePageObject::dirty (void) const
{
    return m_bDirty;
}

inline void OStorePageObject::clean (void)
{
    m_bDirty = sal_False;
}

inline void OStorePageObject::touch (void)
{
    m_bDirty = sal_True;
}

inline sal_uInt32 OStorePageObject::location (void) const
{
    return m_rPage.location();
}

inline void OStorePageObject::location (sal_uInt32 nAddr)
{
    m_rPage.location (nAddr);
    touch();
}

#define STORE_METHOD_ENTER(pMutex) \
    if ((pMutex)) (pMutex)->acquire()

#define STORE_METHOD_LEAVE(pMutex, eErrCode) \
{ \
    if ((pMutex)) (pMutex)->release(); \
    return (eErrCode); \
}

/*========================================================================
 *
 * OStorePageBIOS.
 *
 *======================================================================*/
struct OStoreSuperBlockPage;
struct OStorePageACL;

class OStorePageBIOS : public store::OStoreObject
{
public:
    /** Construction.
     */
    OStorePageBIOS (void);

    /** Conversion into Mutex&
     */
    inline operator osl::Mutex& (void) const;

    /** Initialization.
     *  @param  pLockBytes [in]
     *  @param  eAccessMode [in]
     *  @return store_E_None upon success
     */
    virtual storeError initialize (
        ILockBytes      *pLockBytes,
        storeAccessMode  eAccessMode);

    /** getPageSize.
     */
    storeError getPageSize (sal_uInt16 &rnPageSize);

    /** acquireLock.
     */
    storeError acquireLock (
        sal_uInt32 nAddr, sal_uInt32 nSize);

    /** releaseLock.
     */
    storeError releaseLock (
        sal_uInt32 nAddr, sal_uInt32 nSize);

    /** read.
     */
    storeError read (
        sal_uInt32 nAddr, void *pData, sal_uInt32 nSize);

    /** write.
     */
    storeError write (
        sal_uInt32 nAddr, const void *pData, sal_uInt32 nSize);

    /** isModified.
     */
    inline sal_Bool isModified (void) const;

    /** isWriteable.
     */
    inline sal_Bool isWriteable (void) const;

    /** isValid.
     */
    inline sal_Bool isValid (void) const;

    /** Page Access.
     */
    storeError acquirePage (
        const OStorePageDescriptor& rDescr, storeAccessMode eMode);

    storeError releasePage (const OStorePageDescriptor& rDescr);

    sal_uInt32 getRefererCount (void);

    /** Page Allocation.
     */
    enum Allocation
    {
        ALLOCATE_FIRST = 0,
        ALLOCATE_BEST  = 1,
        ALLOCATE_EOF   = 2
    };

    storeError allocate (
        OStorePageObject& rPage, Allocation eAllocation = ALLOCATE_FIRST);

    virtual storeError free (
        OStorePageObject& rPage);

    /** Page I/O.
     */
    virtual storeError load (
        OStorePageObject& rPage);

    virtual storeError save (
        OStorePageObject& rPage);

    /** close.
     *  @return store_E_None upon success.
     */
    virtual storeError close (void);

    /** flush.
     *  @return store_E_None upon success.
     */
    virtual storeError flush (void);

    /** size.
     */
    storeError size (sal_uInt32 &rnSize);

    /** ScanContext.
     */
    struct ScanContext
    {
        /** Representation.
         */
        OStorePageDescriptor m_aDescr;
        sal_uInt32           m_nSize;
        sal_uInt32           m_nMagic;

        /** Construction.
         */
        inline ScanContext (void);

        /** isValid.
         */
        inline sal_Bool isValid (void) const;
    };

    /** scanBegin.
     */
    storeError scanBegin (
        ScanContext &rCtx,
        sal_uInt32   nMagic = 0);

    /** scanNext.
     */
    storeError scanNext (
        ScanContext      &rCtx,
        OStorePageObject &rPage);

protected:
    /** Destruction (OReference).
     */
    virtual ~OStorePageBIOS (void);

    /** create (SuperBlock).
     */
    storeError create (
        sal_uInt16 nPageSize = STORE_DEFAULT_PAGESIZE);

    /** Page Maintenance.
     */
    storeError peek (
        OStorePageData &rData);
    storeError poke (
        OStorePageData &rData);
    storeError poke (
        OStorePageObject &rPage);

private:
    /** Representation.
     */
    rtl::Reference<ILockBytes>    m_xLockBytes;
    osl::Mutex                    m_aMutex;
    OStorePageACL                *m_pAcl;

    typedef OStoreSuperBlockPage  SuperPage;
    SuperPage                    *m_pSuper;

    sal_Bool                      m_bModified  : 1;
    sal_Bool                      m_bWriteable : 1;

    /** SuperBlock verification and repair.
     */
    storeError verify (SuperPage *&rpSuper);
    storeError repair (SuperPage *&rpSuper);

    /** Not implemented.
     */
    OStorePageBIOS (const OStorePageBIOS&);
    OStorePageBIOS& operator= (const OStorePageBIOS&);
};

inline OStorePageBIOS::operator osl::Mutex& (void) const
{
    return (osl::Mutex&)m_aMutex;
}
inline sal_Bool OStorePageBIOS::isModified (void) const
{
    return m_bModified;
}
inline sal_Bool OStorePageBIOS::isWriteable (void) const
{
    return m_bWriteable;
}
inline sal_Bool OStorePageBIOS::isValid (void) const
{
    return m_xLockBytes.is();
}

inline OStorePageBIOS::ScanContext::ScanContext (void)
    : m_aDescr (0, 0, 0), m_nSize (0), m_nMagic (0)
{
}
inline sal_Bool OStorePageBIOS::ScanContext::isValid (void) const
{
    return (m_aDescr.m_nAddr < m_nSize);
}

/*========================================================================
 *
 * The End.
 *
 *======================================================================*/

} // namespace store

#endif /* !_STORE_STORBASE_HXX_ */

