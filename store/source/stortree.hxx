/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: stortree.hxx,v $
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

#ifndef _STORE_STORTREE_HXX
#define _STORE_STORTREE_HXX "$Revision: 1.6 $"

#include <sal/types.h>
#include <rtl/memory.h>
#include <osl/endian.h>
#include <osl/mutex.hxx>
#include <store/types.h>

#ifndef _STORE_STORBASE_HXX
#include <storbase.hxx>
#endif

namespace store
{

/*========================================================================
 *
 * OStoreBTreeEntry.
 *
 *======================================================================*/
struct OStoreBTreeEntry
{
    typedef OStorePageKey  K;
    typedef OStorePageLink L;

    /** Representation.
    */
    K          m_aKey;
    L          m_aLink;
    sal_uInt32 m_nAttrib;

    /** Construction.
    */
    OStoreBTreeEntry (void)
        : m_nAttrib (0)
    {}

    OStoreBTreeEntry (const OStoreBTreeEntry& rOther)
        : m_aKey    (rOther.m_aKey),
          m_aLink   (rOther.m_aLink),
          m_nAttrib (rOther.m_nAttrib)
    {}

    OStoreBTreeEntry& operator= (const OStoreBTreeEntry& rOther)
    {
        m_aKey    = rOther.m_aKey;
        m_aLink   = rOther.m_aLink;
        m_nAttrib = rOther.m_nAttrib;
        return *this;
    }

    /** Comparison.
    */
    enum CompareResult
    {
        COMPARE_LESS    = -1,
        COMPARE_EQUAL   =  0,
        COMPARE_GREATER =  1
    };

    CompareResult compare (const OStoreBTreeEntry& rOther) const
    {
        if (m_aKey < rOther.m_aKey)
            return COMPARE_LESS;
        else if (m_aKey == rOther.m_aKey)
            return COMPARE_EQUAL;
        else
            return COMPARE_GREATER;
    }

    /** swap (internal and external representation).
    */
    void swap (void)
    {
#ifdef OSL_BIGENDIAN
        m_aKey.swap();
        m_aLink.swap();
        m_nAttrib = OSL_SWAPDWORD(m_nAttrib);
#endif /* OSL_BIGENDIAN */
    }
};

/*========================================================================
 *
 * OStoreBTreeNodeData.
 *
 *======================================================================*/
#define STORE_MAGIC_BTREENODE sal_uInt32(0x58190322)

struct OStoreBTreeNodeData : public store::OStorePageData
{
    typedef OStorePageData      base;
    typedef OStoreBTreeNodeData self;

    typedef OStorePageGuard     G;
    typedef OStoreBTreeEntry    T;

    /** Representation.
    */
    G m_aGuard;
    T m_pData[1];

    /** size.
    */
    static sal_uInt16 size (void)
    {
        return sal_uInt16(sizeof(G));
    }

    /** capacity.
    */
    static sal_uInt16 capacity (const D& rDescr)
    {
        return (rDescr.m_nSize - (base::size() + self::size()));
    }
    sal_uInt16 capacity (void) const
    {
        return self::capacity (base::m_aDescr);
    }

    /** capacityCount (must be even).
    */
    sal_uInt16 capacityCount (void) const
    {
        return sal_uInt16(capacity() / sizeof(T));
    }

    /** usage.
    */
    static sal_uInt16 usage (const D& rDescr)
    {
        return (rDescr.m_nUsed - (base::size() + self::size()));
    }
    sal_uInt16 usage (void) const
    {
        return self::usage (base::m_aDescr);
    }

    /** usageCount.
    */
    sal_uInt16 usageCount (void) const
    {
        return sal_uInt16(usage() / sizeof(T));
    }
    void usageCount (sal_uInt16 nCount)
    {
        base::m_aDescr.m_nUsed = base::size() + self::size() +
            sal_uInt16(nCount * sizeof(T));
    }

    /** Construction.
    */
    OStoreBTreeNodeData (sal_uInt16 nPageSize);
    void initialize (void);

    self& operator= (const self& rOther)
    {
        if (this != &rOther)
        {
            base::operator= (rOther);

            m_aGuard = rOther.m_aGuard;
            rtl_copyMemory (m_pData, rOther.m_pData, capacity());
        }
        return *this;
    }

    /** Comparison.
    */
    sal_Bool operator== (const self& rOther) const
    {
        return (base::operator==(rOther) && (m_aGuard == rOther.m_aGuard));
    }

    /** swap (external and internal representation).
    */
    void swap (const D& rDescr);

    /** guard (external representation).
    */
    void guard (const D& rDescr)
    {
        sal_uInt32 nCRC32 = 0;
        nCRC32 = G::crc32 (nCRC32, &m_aGuard.m_nMagic, sizeof(sal_uInt32));
        nCRC32 = G::crc32 (nCRC32, m_pData, capacity(rDescr));
#ifdef OSL_BIGENDIAN
        nCRC32 = OSL_SWAPDWORD(nCRC32);
#endif /* OSL_BIGENDIAN */
        m_aGuard.m_nCRC32 = nCRC32;
    }

    /** verify (external representation).
    */
    storeError verify (const D& rDescr)
    {
        sal_uInt32 nCRC32 = 0;
        nCRC32 = G::crc32 (nCRC32, &m_aGuard.m_nMagic, sizeof(sal_uInt32));
        nCRC32 = G::crc32 (nCRC32, m_pData, capacity(rDescr));
#ifdef OSL_BIGENDIAN
        nCRC32 = OSL_SWAPDWORD(nCRC32);
#endif /* OSL_BIGENDIAN */
        if (m_aGuard.m_nCRC32 != nCRC32)
            return store_E_InvalidChecksum;
        else
            return store_E_None;
    }

    /** depth.
    */
    sal_uInt32 depth (void) const
    {
        return self::m_aGuard.m_nMagic;
    }
    void depth (sal_uInt32 nDepth)
    {
        self::m_aGuard.m_nMagic = nDepth;
    }

    /** queryMerge.
    */
    sal_Bool queryMerge (const self &rPageR) const
    {
        return ((usageCount() + rPageR.usageCount()) <= capacityCount());
    }

    /** querySplit.
    */
    sal_Bool querySplit (void) const
    {
        return (!(usageCount() < capacityCount()));
    }

    /** Operation.
    */
    sal_uInt16 find   (const T& t) const;
    void       insert (sal_uInt16 i, const T& t);
    void       remove (sal_uInt16 i);

    /** merge (with right page).
     */
    void merge (const self& rPageR);

    /** split (left half copied from right half of left page).
    */
    void split (const self& rPageL);

    /** truncate (to n elements).
    */
    void truncate (sal_uInt16 n);
};

/*========================================================================
 *
 * OStoreBTreeNodeObject.
 *
 *======================================================================*/
class OStoreBTreeNodeObject : public store::OStorePageObject
{
    typedef OStorePageObject      base;
    typedef OStoreBTreeNodeObject self;
    typedef OStoreBTreeNodeData   page;

    typedef OStorePageDescriptor  D;
    typedef OStoreBTreeEntry      T;

public:
    /** Construction.
    */
    inline OStoreBTreeNodeObject (page& rPage);

    /** External representation.
    */
    virtual void       swap   (const D& rDescr);
    virtual void       guard  (const D& rDescr);
    virtual storeError verify (const D& rDescr);

    /** Query split.
    */
    inline sal_Bool querySplit (void) const;

    /** split.
    */
    virtual storeError split (
        sal_uInt16             nIndexL,
        OStoreBTreeNodeData   &rPageL,
        OStoreBTreeNodeData   &rPageR,
        OStorePageBIOS        &rBIOS,
        osl::Mutex            *pMutex = NULL);

    /** remove (down to leaf node, recursive).
    */
    storeError remove (
        sal_uInt16             nIndexL,
        OStoreBTreeEntry      &rEntryL,
        OStoreBTreeNodeData   &rPageL,
#if 0   /* NYI */
        OStoreBTreeNodeData   &rPageR,
#endif  /* NYI */
        OStorePageBIOS        &rBIOS,
        osl::Mutex            *pMutex = NULL);

private:
    /** Representation.
    */
    page& m_rPage;
};

inline OStoreBTreeNodeObject::OStoreBTreeNodeObject (page& rPage)
    : OStorePageObject (rPage), m_rPage (rPage)
{
}

inline sal_Bool OStoreBTreeNodeObject::querySplit (void) const
{
    return m_rPage.querySplit();
}

/*========================================================================
 *
 * OStoreBTreeRootObject.
 *
 *======================================================================*/
class OStoreBTreeRootObject : public store::OStoreBTreeNodeObject
{
    typedef OStoreBTreeNodeObject base;
    typedef OStoreBTreeNodeData   page;

public:
    /** Construction.
    */
    inline OStoreBTreeRootObject (page& rPage);

    /** split.
    */
    virtual storeError split (
        sal_uInt16             nIndexL,
        OStoreBTreeNodeData   &rPageL,
        OStoreBTreeNodeData   &rPageR,
        OStorePageBIOS        &rBIOS,
        osl::Mutex            *pMutex = NULL);

private:
    /** Representation.
    */
    page& m_rPage;

    /** change (Root).
    */
    storeError change (
        OStoreBTreeNodeData   &rPageL,
        OStorePageBIOS        &rBIOS,
        osl::Mutex            *pMutex = NULL);
};

inline OStoreBTreeRootObject::OStoreBTreeRootObject (page& rPage)
    : OStoreBTreeNodeObject (rPage), m_rPage (rPage)
{
}

/*========================================================================
 *
 * The End.
 *
 *======================================================================*/

} // namespace store

#endif /* !_STORE_STORTREE_HXX */

