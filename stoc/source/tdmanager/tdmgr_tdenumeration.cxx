/*************************************************************************
 *
 *  $RCSfile: tdmgr_tdenumeration.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: kso $ $Date: 2002-11-11 08:33:37 $
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
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _STOC_TDMGR_COMMON_HXX
#include "tdmgr_common.hxx"
#endif
#ifndef _STOC_TDMGR_TDENUMERATION_HXX
#include "tdmgr_tdenumeration.hxx"
#endif

using namespace com::sun::star;

namespace stoc_tdmgr
{

//=========================================================================
//=========================================================================
//
// TypeDescriptionEnumerationImpl Implementation.
//
//=========================================================================
//=========================================================================

TypeDescriptionEnumerationImpl::TypeDescriptionEnumerationImpl(
        const rtl::OUString & rModuleName,
        const com::sun::star::uno::Sequence<
            com::sun::star::uno::TypeClass > & rTypes,
        com::sun::star::reflection::TypeDescriptionSearchDepth eDepth,
        const TDEnumerationAccessStack & rTDEAS )
: m_aModuleName( rModuleName ),
  m_aTypes( rTypes ),
  m_eDepth( eDepth ),
  m_aChildren( rTDEAS )
{
    g_moduleCount.modCnt.acquire( &g_moduleCount.modCnt );
}

//=========================================================================
// virtual
TypeDescriptionEnumerationImpl::~TypeDescriptionEnumerationImpl()
{
    g_moduleCount.modCnt.release( &g_moduleCount.modCnt );
}

//=========================================================================
//
// XEnumeration (base of XTypeDescriptionEnumeration) methods
//
//=========================================================================

// virtual
sal_Bool SAL_CALL TypeDescriptionEnumerationImpl::hasMoreElements()
    throw ( uno::RuntimeException )
{
    uno::Reference< reflection::XTypeDescriptionEnumeration > xEnum
        = queryCurrentChildEnumeration();
    if ( xEnum.is() )
        return xEnum->hasMoreElements();

    return sal_False;
}

//=========================================================================
// virtual
uno::Any SAL_CALL TypeDescriptionEnumerationImpl::nextElement()
    throw ( container::NoSuchElementException,
            lang::WrappedTargetException,
            uno::RuntimeException )
{
    uno::Reference< reflection::XTypeDescriptionEnumeration > xEnum
        = queryCurrentChildEnumeration();
    if ( xEnum.is() )
        return xEnum->nextElement();

    throw container::NoSuchElementException(
        rtl::OUString::createFromAscii(
            "No further elements in enumeration!" ),
        static_cast< cppu::OWeakObject * >( this  ) );
}

//=========================================================================
//
// XTypeDescriptionEnumeration methods
//
//=========================================================================

// virtual
uno::Reference< reflection::XTypeDescription > SAL_CALL
TypeDescriptionEnumerationImpl::nextTypeDescription()
    throw ( container::NoSuchElementException,
            uno::RuntimeException )
{
    uno::Reference< reflection::XTypeDescriptionEnumeration > xEnum
        = queryCurrentChildEnumeration();
    if ( xEnum.is() )
        return xEnum->nextTypeDescription();

    throw container::NoSuchElementException(
        rtl::OUString::createFromAscii(
            "No further elements in enumeration!" ),
        static_cast< cppu::OWeakObject * >( this  ) );
}

//=========================================================================
uno::Reference< reflection::XTypeDescriptionEnumeration >
TypeDescriptionEnumerationImpl::queryCurrentChildEnumeration()
{
    osl::MutexGuard aGuard( m_aMutex );

    for (;;)
    {
        if ( m_xEnum.is() )
        {
            if ( m_xEnum->hasMoreElements() )
            {
                return m_xEnum;
            }
            else
            {
                // Forget about enumeration without further elements. Try next.
                m_xEnum.clear();
            }
        }

        // Note: m_xEnum is always null here.

        if ( m_aChildren.empty() )
        {
            // No child enumerations left.
            return m_xEnum;
        }

        try
        {
            m_xEnum =
                m_aChildren.top()->createTypeDescriptionEnumeration(
                    m_aModuleName, m_aTypes, m_eDepth );
        }
        catch ( reflection::NoSuchTypeNameException const & )
        {
            OSL_ENSURE( sal_False,
               "TypeDescriptionEnumerationImpl::queryCurrentChildEnumeration "
               "- Caught NoSuchTypeNameException!" );
        }
        catch ( reflection::InvalidTypeNameException const & )
        {
            OSL_ENSURE( sal_False,
               "TypeDescriptionEnumerationImpl::queryCurrentChildEnumeration "
               "- Caught InvalidTypeNameException!" );
        }

        // We're done with this enumeration access in any case (Either
        // enumeration was successfully created or creation failed for some
        // reason).
        m_aChildren.pop();
    }

    // unreachable
}

} // namespace stoc_tdmgr

