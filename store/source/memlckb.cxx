/*************************************************************************
 *
 *  $RCSfile: memlckb.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2003-03-27 14:06:34 $
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
 *  Contributor(s): Matthias Huetsch <matthias.huetsch@sun.com>
 *
 *
 ************************************************************************/

#include <store/memlckb.hxx>

#ifndef INCLUDED_STDDEF_H
#include <stddef.h>
#define INCLUDED_STDDEF_H
#endif

#ifndef INCLUDED_STRING_H
#include <string.h>
#define INCLUDED_STRING_H
#endif

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif
#ifndef _SAL_MACROS_H_
#include <sal/macros.h>
#endif

#ifndef _RTL_ALLOC_H_
#include <rtl/alloc.h>
#endif

#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif

#ifndef _STORE_TYPES_H_
#include <store/types.h>
#endif
#ifndef _STORE_OBJECT_HXX_
#include <store/object.hxx>
#endif
#ifndef _STORE_LOCKBYTE_HXX_
#include <store/lockbyte.hxx>
#endif

using namespace store;

/*========================================================================
 *
 * OMemoryLockBytes internals.
 *
 *======================================================================*/
#ifdef DEBUG
#define inline static
#endif /* DEBUG */

/*
 * __store_memcpy.
 */
inline void __store_memcpy (void * dst, const void * src, sal_uInt32 n)
{
    ::memcpy (dst, src, n);
}

/*
 * __store_memset.
 */
inline void __store_memset (void * dst, int val, sal_uInt32 n)
{
    ::memset (dst, val, n);
}

#ifdef DEBUG
#ifdef inline
#undef inline
#endif
#define inline
#endif /* DEBUG */

/*========================================================================
 *
 * OMemoryLockBytes_Impl interface.
 *
 *======================================================================*/
namespace store
{

class OMemoryLockBytes_Impl
{
    sal_uInt8  *m_pBuffer;
    sal_uInt32  m_nSize;

public:
    static void * operator new (size_t n) SAL_THROW(())
    {
        return rtl_allocateMemory (sal_uInt32(n));
    }
    static void operator delete (void * p, size_t) SAL_THROW(())
    {
        rtl_freeMemory (p);
    }

    OMemoryLockBytes_Impl (void);
    ~OMemoryLockBytes_Impl (void);

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

    storeError stat (sal_uInt32 &rnSize);
};

} // namespace store

/*========================================================================
 *
 * OMemoryLockBytes_Impl (inline) implementation.
 *
 *======================================================================*/
/*
 * OMemoryLockBytes_Impl.
 */
inline OMemoryLockBytes_Impl::OMemoryLockBytes_Impl (void)
    : m_pBuffer (0), m_nSize (0)
{
}

/*
 * ~OMemoryLockBytes_Impl.
 */
inline OMemoryLockBytes_Impl::~OMemoryLockBytes_Impl (void)
{
    rtl_freeMemory (m_pBuffer);
}

/*
 * resize.
 */
inline storeError OMemoryLockBytes_Impl::resize (sal_uInt32 nSize)
{
    if (!(nSize == m_nSize))
    {
        m_pBuffer = (sal_uInt8*)(rtl_reallocateMemory (m_pBuffer, nSize));
        if (!m_pBuffer)
        {
            m_nSize = 0;
            if (nSize > 0)
                return store_E_OutOfMemory;
            else
                return store_E_None;
        }

        if (nSize > m_nSize)
            __store_memset (m_pBuffer + m_nSize, 0, nSize - m_nSize);
        m_nSize = nSize;
    }
    return store_E_None;
}

/*
 * readAt.
 */
inline storeError OMemoryLockBytes_Impl::readAt (
    sal_uInt32  nOffset,
    void       *pBuffer,
    sal_uInt32  nBytes,
    sal_uInt32 &rnDone)
{
    if (m_pBuffer)
    {
        if (!(nOffset < m_nSize))
            return store_E_None;

        nBytes = SAL_MIN(nOffset + nBytes, m_nSize) - nOffset;
        if (!(nBytes > 0))
            return store_E_None;

        __store_memcpy (pBuffer, m_pBuffer + nOffset, nBytes);
        rnDone = nBytes;
    }
    return store_E_None;
}

/*
 * writeAt.
 */
inline storeError OMemoryLockBytes_Impl::writeAt (
        sal_uInt32  nOffset,
        const void *pBuffer,
        sal_uInt32  nBytes,
        sal_uInt32 &rnDone)
{
    if (m_nSize < (nOffset + nBytes))
    {
        storeError eErrCode = resize (nOffset + nBytes);
        if (eErrCode != store_E_None)
            return eErrCode;
    }

    __store_memcpy (m_pBuffer + nOffset, pBuffer, nBytes);
    rnDone = nBytes;

    return store_E_None;
}

/*
 * stat.
 */
inline storeError OMemoryLockBytes_Impl::stat (sal_uInt32 &rnSize)
{
    rnSize = m_nSize;
    return store_E_None;
}

/*========================================================================
 *
 * OMemoryLockBytes implementation.
 *
 *======================================================================*/
/*
 * OMemoryLockBytes.
 */
OMemoryLockBytes::OMemoryLockBytes (void)
    : m_pImpl (new OMemoryLockBytes_Impl())
{
}

/*
 * ~OMemoryLockBytes.
 */
OMemoryLockBytes::~OMemoryLockBytes (void)
{
    delete m_pImpl;
}

/*
 * acquire.
 */
oslInterlockedCount SAL_CALL OMemoryLockBytes::acquire (void)
{
    return OStoreObject::acquire();
}

/*
 * release.
 */
oslInterlockedCount SAL_CALL OMemoryLockBytes::release (void)
{
    return OStoreObject::release();
}

/*
 * readAt.
 */
storeError OMemoryLockBytes::readAt (
    sal_uInt32  nOffset,
    void       *pBuffer,
    sal_uInt32  nBytes,
    sal_uInt32 &rnDone)
{
    // Initialize [out] param.
    rnDone = 0;

    // Acquire exclusive access.
    osl::MutexGuard aGuard (m_aMutex);
    return m_pImpl->readAt (nOffset, pBuffer, nBytes, rnDone);
}

/*
 * writeAt.
 */
storeError OMemoryLockBytes::writeAt (
    sal_uInt32  nOffset,
    const void *pBuffer,
    sal_uInt32  nBytes,
    sal_uInt32 &rnDone)
{
    // Initialize [out] param.
    rnDone = 0;

    // Acquire exclusive access.
    osl::MutexGuard aGuard (m_aMutex);
    return m_pImpl->writeAt (nOffset, pBuffer, nBytes, rnDone);
}

/*
 * flush.
 */
storeError OMemoryLockBytes::flush (void)
{
    // Acquire exclusive access.
    osl::MutexGuard aGuard (m_aMutex);
    return store_E_None;
}

/*
 * setSize.
 */
storeError OMemoryLockBytes::setSize (sal_uInt32 nSize)
{
    // Acquire exclusive access.
    osl::MutexGuard aGuard (m_aMutex);
    return m_pImpl->resize (nSize);
}

/*
 * stat.
 */
storeError OMemoryLockBytes::stat (sal_uInt32 &rnSize)
{
    // Initialize [out] param.
    rnSize = 0;

    // Acquire exclusive access.
    osl::MutexGuard aGuard (m_aMutex);
    return m_pImpl->stat (rnSize);
}

/*
 * lockRange.
 */
storeError OMemoryLockBytes::lockRange (
    sal_uInt32 nOffset, sal_uInt32 nBytes)
{
    // Acquire exclusive access.
    osl::MutexGuard aGuard (m_aMutex);
    return store_E_None; // E_Unsupported
}

/*
 * unlockRange.
 */
storeError OMemoryLockBytes::unlockRange (
    sal_uInt32 nOffset, sal_uInt32 nBytes)
{
    // Acquire exclusive access.
    osl::MutexGuard aGuard (m_aMutex);
    return store_E_None; // E_Unsupported
}

