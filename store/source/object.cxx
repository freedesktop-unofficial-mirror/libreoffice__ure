/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: object.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 01:11:37 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_store.hxx"

#define _STORE_OBJECT_CXX_ "$Revision: 1.4 $"

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#ifndef _RTL_ALLOC_H_
#include <rtl/alloc.h>
#endif
#ifndef _RTL_REF_HXX_
#include <rtl/ref.hxx>
#endif

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _OSL_INTERLCK_H_
#include <osl/interlck.h>
#endif

#ifndef _STORE_OBJECT_HXX_
#include <store/object.hxx>
#endif

namespace store
{

/*========================================================================
 *
 * OStoreObject implementation.
 *
 *======================================================================*/
const sal_uInt32 OStoreObject::m_nTypeId = sal_uInt32(0x58190322);

/*
 * OStoreObject.
 */
OStoreObject::OStoreObject (void)
    : m_nRefCount (0)
{
}

/*
 * ~OStoreObject.
 */
OStoreObject::~OStoreObject (void)
{
    OSL_ASSERT(m_nRefCount == 0);
}

/*
 * operator new.
 */
void* OStoreObject::operator new (size_t n)
{
    return rtl_allocateMemory (n);
}

/*
 * operator delete.
 */
void OStoreObject::operator delete (void *p)
{
    rtl_freeMemory (p);
}

/*
 * isKindOf.
 */
sal_Bool SAL_CALL OStoreObject::isKindOf (sal_uInt32 nTypeId)
{
    return (nTypeId == m_nTypeId);
}

/*
 * acquire.
 */
oslInterlockedCount SAL_CALL OStoreObject::acquire (void)
{
    oslInterlockedCount result = osl_incrementInterlockedCount (&m_nRefCount);
    return (result);
}

/*
 * release.
 */
oslInterlockedCount SAL_CALL OStoreObject::release (void)
{
    oslInterlockedCount result = osl_decrementInterlockedCount (&m_nRefCount);
    if (result == 0)
    {
        // Last reference released.
        delete this;
    }
    return (result);
}

} // namespace store
