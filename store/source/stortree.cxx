/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: stortree.cxx,v $
 * $Revision: 1.8 $
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

#include "stortree.hxx"

#include "sal/types.h"
#include "osl/diagnose.h"

#include "store/types.h"

#include "storbase.hxx"
#include "storbios.hxx"

using namespace store;

/*========================================================================
 *
 * OStoreBTreeNodeData implementation.
 *
 *======================================================================*/
/*
 * OStoreBTreeNodeData.
 */
OStoreBTreeNodeData::OStoreBTreeNodeData (sal_uInt16 nPageSize)
    : OStorePageData (nPageSize)
{
    base::m_aGuard.m_nMagic = store::htonl(self::theTypeId);
    base::m_aDescr.m_nUsed  = store::htons(self::thePageSize); // usageCount(0)
    self::m_aGuard.m_nMagic = store::htonl(0); // depth(0)

    sal_uInt16 const n = capacityCount();
    T const          t;

    for (sal_uInt16 i = 1; i < n; i++)
        m_pData[i] = t;
}

/*
 * find.
 */
sal_uInt16 OStoreBTreeNodeData::find (const T& t) const
{
    register sal_Int32 l = 0;
    register sal_Int32 r = usageCount() - 1;

    while (l < r)
    {
        register sal_Int32 const m = ((l + r) >> 1);

        if (t.m_aKey == m_pData[m].m_aKey)
            return ((sal_uInt16)(m));
        if (t.m_aKey < m_pData[m].m_aKey)
            r = m - 1;
        else
            l = m + 1;
    }

    sal_uInt16 const k = ((sal_uInt16)(r));
    if ((k < capacityCount()) && (t.m_aKey < m_pData[k].m_aKey))
        return(k - 1);
    else
        return(k);
}

/*
 * insert.
 */
void OStoreBTreeNodeData::insert (sal_uInt16 i, const T& t)
{
    sal_uInt16 const n = usageCount();
    sal_uInt16 const m = capacityCount();
    if ((n < m) && (i < m))
    {
        // shift right.
        memmove (&(m_pData[i + 1]), &(m_pData[i]), (n - i) * sizeof(T));

        // insert.
        m_pData[i] = t;
        usageCount (n + 1);
    }
}

/*
 * remove.
 */
void OStoreBTreeNodeData::remove (sal_uInt16 i)
{
    sal_uInt16 const n = usageCount();
    if (i < n)
    {
        // shift left.
        memmove (&(m_pData[i]), &(m_pData[i + 1]), (n - i - 1) * sizeof(T));

        // truncate.
        m_pData[n - 1] = T();
        usageCount (n - 1);
    }
}

/*
 * merge (with right page).
 */
void OStoreBTreeNodeData::merge (const self& rPageR)
{
    sal_uInt16 const n = usageCount();
    sal_uInt16 const m = rPageR.usageCount();
    if ((n + m) <= capacityCount())
    {
        memcpy (&(m_pData[n]), &(rPageR.m_pData[0]), m * sizeof(T));
        usageCount (n + m);
    }
}

/*
 * split (left half copied from right half of left page).
 */
void OStoreBTreeNodeData::split (const self& rPageL)
{
    sal_uInt16 const h = capacityCount() / 2;
    memcpy (&(m_pData[0]), &(rPageL.m_pData[h]), h * sizeof(T));
    truncate (h);
}

/*
 * truncate.
 */
void OStoreBTreeNodeData::truncate (sal_uInt16 n)
{
    sal_uInt16 const m = capacityCount();
    T const          t;

    for (sal_uInt16 i = n; i < m; i++)
        m_pData[i] = t;
    usageCount (n);
}

/*========================================================================
 *
 * OStoreBTreeNodeObject implementation.
 *
 *======================================================================*/
/*
 * guard.
 */
storeError OStoreBTreeNodeObject::guard (sal_uInt32 nAddr)
{
    return PageHolderObject< page >::guard (m_xPage, nAddr);
}

/*
 * verify.
 */
storeError OStoreBTreeNodeObject::verify (sal_uInt32 nAddr) const
{
    return PageHolderObject< page >::verify (m_xPage, nAddr);
}

/*
 * split.
 */
storeError OStoreBTreeNodeObject::split (
    sal_uInt16                 nIndexL,
    PageHolderObject< page > & rxPageL,
    OStorePageBIOS           & rBIOS)
{
    PageHolderObject< page > xPage (m_xPage);
    if (!xPage.is())
        return store_E_InvalidAccess;

    // Check left page usage.
    if (!rxPageL.is())
        return store_E_InvalidAccess;
    if (!rxPageL->querySplit())
        return store_E_None;

    // Save PageDescriptor.
    OStorePageDescriptor aDescr (xPage->m_aDescr);
    aDescr.m_nAddr = store::ntohl(aDescr.m_nAddr);
    aDescr.m_nSize = store::ntohs(aDescr.m_nSize);

    // Acquire Lock.
    storeError eErrCode = rBIOS.acquireLock (aDescr.m_nAddr, aDescr.m_nSize);
    if (eErrCode != store_E_None)
        return eErrCode;

    // [Begin PageL Lock (NYI)]

    // Construct right page.
    PageHolderObject< page > xPageR;
    if (!xPageR.construct (rBIOS.allocator()))
    {
        rBIOS.releaseLock (aDescr.m_nAddr, aDescr.m_nSize);
        return store_E_OutOfMemory;
    }

    // Split right page off left page.
    xPageR->split (*rxPageL);
    xPageR->depth (rxPageL->depth());

    // Allocate right page.
    self aNodeR (xPageR.get());
    eErrCode = rBIOS.allocate (aNodeR);
    if (eErrCode != store_E_None)
    {
        rBIOS.releaseLock (aDescr.m_nAddr, aDescr.m_nSize);
        return eErrCode;
    }

    // Truncate left page.
    rxPageL->truncate (rxPageL->capacityCount() / 2);

    // Save left page.
    self aNodeL (rxPageL.get());
    eErrCode = rBIOS.saveObjectAt (aNodeL, aNodeL.location());
    if (eErrCode != store_E_None)
    {
        // Must not happen.
        OSL_TRACE("OStoreBTreeNodeObject::split(): save() failed");

        // Release Lock and Leave.
        rBIOS.releaseLock (aDescr.m_nAddr, aDescr.m_nSize);
        return eErrCode;
    }

    // [End PageL Lock (NYI)]

    // Insert right page.
    OStorePageLink aLink (xPageR->location());
    xPage->insert (nIndexL + 1, T(xPageR->m_pData[0].m_aKey, aLink));

    // Save this page.
    eErrCode = rBIOS.saveObjectAt (*this, location());
    if (eErrCode != store_E_None)
    {
        // Must not happen.
        OSL_TRACE("OStoreBTreeNodeObject::split(): save() failed");

        // Release Lock and Leave.
        rBIOS.releaseLock (aDescr.m_nAddr, aDescr.m_nSize);
        return eErrCode;
    }

    // Release Lock and Leave.
    return rBIOS.releaseLock (aDescr.m_nAddr, aDescr.m_nSize);
}

/*
 * remove (down to leaf node, recursive).
 */
storeError OStoreBTreeNodeObject::remove (
    sal_uInt16         nIndexL,
    OStoreBTreeEntry & rEntryL,
    OStorePageBIOS &   rBIOS)
{
    PageHolderObject< page > xImpl (m_xPage);
    page & rPage = (*xImpl);

    // Save PageDescriptor.
    OStorePageDescriptor aDescr (rPage.m_aDescr);
    aDescr.m_nAddr = store::ntohl(aDescr.m_nAddr);
    aDescr.m_nSize = store::ntohs(aDescr.m_nSize);

    // Acquire Lock.
    storeError eErrCode = rBIOS.acquireLock (aDescr.m_nAddr, aDescr.m_nSize);
    if (eErrCode != store_E_None)
        return eErrCode;

    // Check depth.
    if (rPage.depth())
    {
        // Check link entry.
        T const aEntryL (rPage.m_pData[nIndexL]);
        if (!(rEntryL.compare (aEntryL) == T::COMPARE_EQUAL))
        {
            rBIOS.releaseLock (aDescr.m_nAddr, aDescr.m_nSize);
            return store_E_InvalidAccess;
        }

        // Load link node.
        self aNodeL;
        eErrCode = rBIOS.loadObjectAt (aNodeL, aEntryL.m_aLink.location());
        if (eErrCode != store_E_None)
        {
            rBIOS.releaseLock (aDescr.m_nAddr, aDescr.m_nSize);
            return eErrCode;
        }

        // Recurse: remove from link node.
        eErrCode = aNodeL.remove (0, rEntryL, rBIOS);
        if (eErrCode != store_E_None)
        {
            rBIOS.releaseLock (aDescr.m_nAddr, aDescr.m_nSize);
            return eErrCode;
        }

        // Check resulting link node usage.
        PageHolderObject< page > xPageL (aNodeL.get());
        if (xPageL->usageCount() == 0)
        {
            // Free empty link node.
            OStorePageData aPageHead;
            eErrCode = rBIOS.free (aPageHead, xPageL->location());
            if (eErrCode != store_E_None)
            {
                rBIOS.releaseLock (aDescr.m_nAddr, aDescr.m_nSize);
                return eErrCode;
            }

            // Remove index.
            rPage.remove (nIndexL);
            touch();
        }
        else
        {
#if 0   /* NYI */
            // Check for right sibling.
            sal_uInt16 const nIndexR = nIndexL + 1;
            if (nIndexR < rPage.usageCount())
            {
                // Load right link node.
                self aNodeR;
                eErrCode = rBIOS.loadObjectAt (aNodeR, rPage.m_pData[nIndexR].m_aLink.location());
                if (eErrCode == store_E_None)
                {
                    if (rPageL.queryMerge (rPageR))
                    {
                        rPageL.merge (rPageR);

                        eErrCode = rBIOS.free (aPageHead, rPageR.location());
                    }
                }
            }
#endif  /* NYI */

            // Relink.
            rPage.m_pData[nIndexL].m_aKey = xPageL->m_pData[0].m_aKey;
            touch();
        }
    }
    else
    {
        // Check leaf entry.
        if (!(rEntryL.compare (rPage.m_pData[nIndexL]) == T::COMPARE_EQUAL))
        {
            rBIOS.releaseLock (aDescr.m_nAddr, aDescr.m_nSize);
            return store_E_NotExists;
        }

        // Save leaf entry.
        rEntryL = rPage.m_pData[nIndexL];

        // Remove leaf index.
        rPage.remove (nIndexL);
        touch();
    }

    // Check for modified node.
    if (dirty())
    {
        // Save this page.
        eErrCode = rBIOS.saveObjectAt (*this, location());
        if (eErrCode != store_E_None)
        {
            // Must not happen.
            OSL_TRACE("OStoreBTreeNodeObject::remove(): save() failed");

            // Release Lock and Leave.
            rBIOS.releaseLock (aDescr.m_nAddr, aDescr.m_nSize);
            return eErrCode;
        }
    }

    // Release Lock and Leave.
    return rBIOS.releaseLock (aDescr.m_nAddr, aDescr.m_nSize);
}

/*========================================================================
 *
 * OStoreBTreeRootObject implementation.
 *
 *======================================================================*/
/*
 * testInvariant.
 * Precond: root node page loaded.
 */
bool OStoreBTreeRootObject::testInvariant (char const * message)
{
    OSL_PRECOND(m_xPage.get() != 0, "OStoreBTreeRootObject::testInvariant(): Null pointer");
    bool result = ((m_xPage->location() - m_xPage->size()) == 0);
    OSL_POSTCOND(result, message); (void) message;
    return result;
}

/*
 * loadOrCreate.
 */
storeError OStoreBTreeRootObject::loadOrCreate (
    sal_uInt32       nAddr,
    OStorePageBIOS & rBIOS)
{
    storeError eErrCode = rBIOS.loadObjectAt (*this, nAddr);
    if (eErrCode != store_E_NotExists)
        return eErrCode;

    eErrCode = construct<page>(rBIOS.allocator());
    if (eErrCode != store_E_None)
        return eErrCode;

    eErrCode = rBIOS.allocate (*this);
    if (eErrCode != store_E_None)
        return eErrCode;

    // Notify caller of the creation.
    (void) testInvariant("OStoreBTreeRootObject::loadOrCreate(): leave");
    return store_E_Pending;
}

/*
 * change.
 */
storeError OStoreBTreeRootObject::change (
    PageHolderObject< page > & rxPageL,
    OStorePageBIOS &           rBIOS)
{
    PageHolderObject< page > xPage (m_xPage);
    (void) testInvariant("OStoreBTreeRootObject::change(): enter");

    // Save PageDescriptor.
    OStorePageDescriptor aDescr (xPage->m_aDescr);
    aDescr.m_nAddr = store::ntohl(aDescr.m_nAddr);
    aDescr.m_nSize = store::ntohs(aDescr.m_nSize);

    // Save root location.
    sal_uInt32 const nRootAddr = xPage->location();

    // Acquire Lock.
    storeError eErrCode = rBIOS.acquireLock (aDescr.m_nAddr, aDescr.m_nSize);
    if (eErrCode != store_E_None)
        return eErrCode;

    // Construct new root.
    if (!rxPageL.construct (rBIOS.allocator()))
    {
        rBIOS.releaseLock (aDescr.m_nAddr, aDescr.m_nSize);
        return store_E_OutOfMemory;
    }

    // Save this as prev root.
    eErrCode = rBIOS.allocate (*this);
    if (eErrCode != store_E_None)
    {
        rBIOS.releaseLock (aDescr.m_nAddr, aDescr.m_nSize);
        return store_E_OutOfMemory;
    }

    // Setup new root.
    rxPageL->depth (xPage->depth() + 1);
    rxPageL->m_pData[0] = xPage->m_pData[0];
    rxPageL->m_pData[0].m_aLink = xPage->location();
    rxPageL->usageCount(1);

    // Change root.
    rxPageL.swap (xPage);
    {
        PageHolder tmp (xPage.get());
        tmp.swap (m_xPage);
    }

    // Save this as new root.
    eErrCode = rBIOS.saveObjectAt (*this, nRootAddr);
    if (eErrCode != store_E_None)
    {
        // Must not happen.
        OSL_TRACE("OStoreBTreeRootObject::change(): save() failed");

        // Release Lock and Leave.
        rBIOS.releaseLock (aDescr.m_nAddr, aDescr.m_nSize);
        return eErrCode;
    }

    // Flush for robustness.
    (void) rBIOS.flush();

    // Done. Release Lock and Leave.
    (void) testInvariant("OStoreBTreeRootObject::change(): leave");
    return rBIOS.releaseLock (aDescr.m_nAddr, aDescr.m_nSize);
}

/*
 * find_lookup (w/o split()).
 * Precond: root node page loaded.
 */
storeError OStoreBTreeRootObject::find_lookup (
    OStoreBTreeNodeObject & rNode,  // [out]
    sal_uInt16 &            rIndex, // [out]
    OStorePageKey const &   rKey,
    OStorePageBIOS &        rBIOS)
{
    // Init node w/ root page.
    (void) testInvariant("OStoreBTreeRootObject::find_lookup(): enter");
    {
        PageHolder tmp (m_xPage);
        tmp.swap (rNode.get());
    }

    // Setup BTree entry.
    T const entry (rKey);

    // Check current page.
    PageHolderObject< page > xPage (rNode.get());
    for (; xPage->depth() > 0; xPage = rNode.makeHolder< page >())
    {
        // Find next page.
        page const & rPage = (*xPage);
        sal_uInt16 const i = rPage.find(entry);
        sal_uInt16 const n = rPage.usageCount();
        if (!(i < n))
        {
            // Path to entry not exists (Must not happen(?)).
            return store_E_NotExists;
        }

        // Check address.
        sal_uInt32 const nAddr = rPage.m_pData[i].m_aLink.location();
        if (nAddr == STORE_PAGE_NULL)
        {
            // Path to entry not exists (Must not happen(?)).
            return store_E_NotExists;
        }

        // Load next page.
        storeError eErrCode = rBIOS.loadObjectAt (rNode, nAddr);
        if (eErrCode != store_E_None)
            return eErrCode;
    }

    // Find index.
    page const & rPage = (*xPage);
    rIndex = rPage.find(entry);
    if (!(rIndex < rPage.usageCount()))
        return store_E_NotExists;

    // Compare entry.
    T::CompareResult eResult = entry.compare(rPage.m_pData[rIndex]);
    OSL_POSTCOND(eResult != T::COMPARE_LESS, "store::BTreeRoot::find_lookup(): sort error");
    if (eResult == T::COMPARE_LESS)
        return store_E_Unknown;

    // Greater or Equal.
    (void) testInvariant("OStoreBTreeRootObject::find_lookup(): leave");
    return store_E_None;
}

/*
 * find_insert (possibly with split()).
 * Precond: root node page loaded.
 */
storeError OStoreBTreeRootObject::find_insert (
    OStoreBTreeNodeObject & rNode,  // [out]
    sal_uInt16 &            rIndex, // [out]
    OStorePageKey const &   rKey,
    OStorePageBIOS &        rBIOS)
{
    (void) testInvariant("OStoreBTreeRootObject::find_insert(): enter");

    // Check for RootNode split.
    PageHolderObject< page > xRoot (m_xPage);
    if (xRoot->querySplit())
    {
        PageHolderObject< page > xPageL;

        // Change root.
        storeError eErrCode = change (xPageL, rBIOS);
        if (eErrCode != store_E_None)
            return eErrCode;

        // Split left page (prev root).
        eErrCode = split (0, xPageL, rBIOS);
        if (eErrCode != store_E_None)
            return eErrCode;
    }

    // Init node w/ root page.
    {
        PageHolder tmp (m_xPage);
        tmp.swap (rNode.get());
    }
    
    // Setup BTree entry.
    T const entry (rKey);

    // Check current Page.
    PageHolderObject< page > xPage (rNode.get());
    for (; xPage->depth() > 0; xPage = rNode.makeHolder< page >())
    {
        // Find next page.
        page const & rPage = (*xPage);
        sal_uInt16 const i = rPage.find (entry);
        sal_uInt16 const n = rPage.usageCount();
        if (!(i < n))
        {
            // Path to entry not exists (Must not happen(?)).
            return store_E_NotExists;
        }

        // Check address.
        sal_uInt32 const nAddr = rPage.m_pData[i].m_aLink.location();
        if (nAddr == STORE_PAGE_NULL)
        {
            // Path to entry not exists (Must not happen(?)).
            return store_E_NotExists;
        }

        // Load next page.
        OStoreBTreeNodeObject aNext;
        storeError eErrCode = rBIOS.loadObjectAt (aNext, nAddr);
        if (eErrCode != store_E_None)
            return eErrCode;

        // Check for next node split.
        PageHolderObject< page > xNext (aNext.get());
        if (xNext->querySplit())
        {
            // Split next node.
            eErrCode = rNode.split (i, xNext, rBIOS);
            if (eErrCode != store_E_None)
                return eErrCode;

            // Restart.
            continue;
        }

        // Let next page be current.
        PageHolder tmp (aNext.get());
        tmp.swap (rNode.get());
    }

    // Find index.
    page const & rPage = (*xPage);
    rIndex = rPage.find(entry);
    if (rIndex < rPage.usageCount())
    {
        // Compare entry.
        T::CompareResult result = entry.compare (rPage.m_pData[rIndex]);
        OSL_POSTCOND(result != T::COMPARE_LESS, "store::BTreeRoot::find_insert(): sort error");
        if (result == T::COMPARE_LESS)
            return store_E_Unknown;

        if (result == T::COMPARE_EQUAL)
            return store_E_AlreadyExists;
    }

    // Greater or not (yet) existing.
    (void) testInvariant("OStoreBTreeRootObject::find_insert(): leave");
    return store_E_None;
}
