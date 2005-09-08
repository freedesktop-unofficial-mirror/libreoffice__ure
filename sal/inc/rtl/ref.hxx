/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ref.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 14:40:48 $
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

#ifndef _RTL_REF_HXX_
#define _RTL_REF_HXX_

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _OSL_INTERLCK_H_
#include <osl/interlck.h>
#endif

namespace rtl
{

/** Interface for a reference type.
*/
class IReference
{
public:
    /** @see osl_incrementInterlockedCount.
     */
    virtual oslInterlockedCount SAL_CALL acquire() = 0;

    /** @see osl_decrementInterlockedCount.
     */
    virtual oslInterlockedCount SAL_CALL release() = 0;
};


/** Template reference class for reference type derived from IReference.
*/
template <class reference_type>
class Reference
{
    /** The <b>reference_type</b> body pointer.
     */
    reference_type * m_pBody;


public:
    /** Constructor...
     */
    inline Reference()
        : m_pBody (0)
    {}


    /** Constructor...
     */
    inline Reference (reference_type * pBody)
        : m_pBody (pBody)
    {
        if (m_pBody)
            m_pBody->acquire();
    }
    

    /** Copy constructor...
     */
    inline Reference (const Reference<reference_type> & handle)
        : m_pBody (handle.m_pBody)
    {
        if (m_pBody)
            m_pBody->acquire();
    }


    /** Destructor...
     */
    inline ~Reference()
    {
        if (m_pBody)
            m_pBody->release();
    }

    /** Set...
         Similar to assignment.
     */
    inline Reference<reference_type> &
    SAL_CALL set (reference_type * pBody)
    {
        if (pBody)
            pBody->acquire();
        if (m_pBody)
            m_pBody->release();
        m_pBody = pBody;
        return *this;
    }

    /** Assignment.
         Unbinds this instance from its body (if bound) and
         bind it to the body represented by the handle.
     */
    inline Reference<reference_type> &
    SAL_CALL operator= (const Reference<reference_type> & handle)
    {
        return set( handle.m_pBody );
    }

    /** Assignment...
     */
    inline Reference<reference_type> &
    SAL_CALL operator= (reference_type * pBody)
    {
        return set( pBody );
    }

    /** Unbind the body from this handle.
         Note that for a handle representing a large body,
         "handle.clear().set(new body());" _might_
         perform a little bit better than "handle.set(new body());",
         since in the second case two large objects exist in memory
         (the old body and the new body).
     */
    inline Reference<reference_type> & SAL_CALL clear()
    {
        if (m_pBody)
        {
            m_pBody->release();
            m_pBody = 0;
        }
        return *this;
    }


    /** Get the body. Can be used instead of operator->().
         I.e. handle->someBodyOp() and handle.get()->someBodyOp()
         are the same.
     */
    inline reference_type * SAL_CALL get() const
    {
        return m_pBody;
    }


    /** Probably most common used: handle->someBodyOp().
     */
    inline reference_type * SAL_CALL operator->() const
    {
        OSL_PRECOND(m_pBody, "Reference::operator->() : null body");
        return m_pBody;
    }


    /** Allows (*handle).someBodyOp().
    */
    inline reference_type & SAL_CALL operator*() const
    {
        OSL_PRECOND(m_pBody, "Reference::operator*() : null body");
        return *m_pBody;
    }


    /** Returns True if the handle does point to a valid body.
     */
    inline sal_Bool SAL_CALL is() const
    {
        return (m_pBody != 0);
    }


    /** Returns True if this points to pBody.
     */
    inline sal_Bool SAL_CALL operator== (const reference_type * pBody) const
    {
        return (m_pBody == pBody);
    }


    /** Returns True if handle points to the same body.
     */
    inline sal_Bool
    SAL_CALL operator== (const Reference<reference_type> & handle) const
    {
        return (m_pBody == handle.m_pBody);
    }


    /** Needed to place References into STL collection.
     */
    inline sal_Bool
    SAL_CALL operator!= (const Reference<reference_type> & handle) const
    {
        return (m_pBody != handle.m_pBody);
    }


    /** Needed to place References into STL collection.
     */
    inline sal_Bool
    SAL_CALL operator< (const Reference<reference_type> & handle) const
    {
        return (m_pBody < handle.m_pBody);
    }


    /** Needed to place References into STL collection.
     */
    inline sal_Bool
    SAL_CALL operator> (const Reference<reference_type> & handle) const
    {
        return (m_pBody > handle.m_pBody);
    }
};


} // namespace rtl

#endif /* !_RTL_REF_HXX_ */
