/*************************************************************************
 *
 *  $RCSfile: UriReference.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: kz $ $Date: 2004-01-19 18:28:25 $
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
 *  Copyright: 2003 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#include "UriReference.hxx"

#include "osl/diagnose.h"
#include "osl/mutex.hxx"
#include "rtl/string.h"
#include "rtl/ustrbuf.hxx"
#include "rtl/ustring.hxx"

namespace css = com::sun::star;
using stoc::uriproc::UriReference;

UriReference::UriReference(
    rtl::OUString const & scheme, bool isHierarchical, bool hasAuthority,
    rtl::OUString const & authority, rtl::OUString const & path,
    bool hasQuery, rtl::OUString const & query):
    m_hasFragment(false)
{
    initialize(scheme, isHierarchical, hasAuthority, authority, path, hasQuery,
               query);
}

rtl::OUString UriReference::getUriReference() throw (css::uno::RuntimeException)
{
    osl::MutexGuard g(m_mutex);
    rtl::OUStringBuffer buf;
    if (m_scheme.getLength() != 0) {
        buf.append(m_scheme);
        buf.append(static_cast< sal_Unicode >(':'));
    }
    appendSchemeSpecificPart(buf);
    if (m_hasFragment) {
        buf.append(static_cast< sal_Unicode >('#'));
        buf.append(m_fragment);
    }
    return buf.makeStringAndClear();
}

sal_Bool UriReference::isAbsolute() throw (css::uno::RuntimeException) {
    return m_scheme.getLength() != 0;
}

rtl::OUString UriReference::getScheme() throw (css::uno::RuntimeException) {
    return m_scheme;
}

rtl::OUString UriReference::getSchemeSpecificPart()
    throw (css::uno::RuntimeException)
{
    osl::MutexGuard g(m_mutex);
    rtl::OUStringBuffer buf;
    appendSchemeSpecificPart(buf);
    return buf.makeStringAndClear();
}

sal_Bool UriReference::isHierarchical() throw (css::uno::RuntimeException) {
    osl::MutexGuard g(m_mutex);
    return m_isHierarchical;
}

sal_Bool UriReference::hasAuthority() throw (css::uno::RuntimeException) {
    osl::MutexGuard g(m_mutex);
    return m_hasAuthority;
}

rtl::OUString UriReference::getAuthority() throw (css::uno::RuntimeException) {
    osl::MutexGuard g(m_mutex);
    return m_authority;
}

rtl::OUString UriReference::getPath() throw (css::uno::RuntimeException) {
    osl::MutexGuard g(m_mutex);
    return m_path;
}

sal_Bool UriReference::hasRelativePath() throw (css::uno::RuntimeException) {
    osl::MutexGuard g(m_mutex);
    return m_isHierarchical && !m_hasAuthority
        && (m_path.getLength() == 0 || m_path[0] != '/');
}

sal_Int32 UriReference::getPathSegmentCount() throw (css::uno::RuntimeException)
{
    osl::MutexGuard g(m_mutex);
    if (!m_isHierarchical || m_path.getLength() == 0) {
        return 0;
    } else {
        sal_Int32 n = m_path[0] == '/' ? 0 : 1;
        for (sal_Int32 i = 0;; ++i) {
            i = m_path.indexOf('/', i);
            if (i < 0) {
                break;
            }
            ++n;
        }
        return n;
    }
}

rtl::OUString UriReference::getPathSegment(sal_Int32 index)
    throw (css::uno::RuntimeException)
{
    osl::MutexGuard g(m_mutex);
    if (m_isHierarchical && m_path.getLength() != 0 && index >= 0) {
        for (sal_Int32 i = m_path[0] == '/' ? 1 : 0;; ++i) {
            if (index-- == 0) {
                sal_Int32 j = m_path.indexOf('/', i);
                return j < 0 ? m_path.copy(i) : m_path.copy(i, j - i);
            }
            i = m_path.indexOf('/', i);
            if (i < 0) {
                break;
            }
        }
    }
    return rtl::OUString();
}

sal_Bool UriReference::hasQuery() throw (css::uno::RuntimeException) {
    osl::MutexGuard g(m_mutex);
    return m_hasQuery;
}

rtl::OUString UriReference::getQuery() throw (css::uno::RuntimeException) {
    osl::MutexGuard g(m_mutex);
    return m_query;
}

sal_Bool UriReference::hasFragment() throw (css::uno::RuntimeException) {
    osl::MutexGuard g(m_mutex);
    return m_hasFragment;
}

rtl::OUString UriReference::getFragment() throw (css::uno::RuntimeException) {
    osl::MutexGuard g(m_mutex);
    return m_fragment;
}

void UriReference::setFragment(rtl::OUString const & fragment)
    throw (css::uno::RuntimeException)
{
    osl::MutexGuard g(m_mutex);
    m_hasFragment = true;
    m_fragment = fragment;
}

void UriReference::clearFragment() throw (css::uno::RuntimeException) {
    osl::MutexGuard g(m_mutex);
    m_hasFragment = false;
    m_fragment = rtl::OUString();
}

UriReference::UriReference():
    m_isHierarchical(true), m_hasAuthority(false), m_hasQuery(false),
    m_hasFragment(false)
{}

UriReference::~UriReference() {}

void UriReference::initialize(
    rtl::OUString const & scheme, bool isHierarchical, bool hasAuthority,
    rtl::OUString const & authority, rtl::OUString const & path,
    bool hasQuery, rtl::OUString const & query)
{
    OSL_ASSERT(scheme.getLength() != 0 || isHierarchical);
    OSL_ASSERT(!hasAuthority || isHierarchical);
    OSL_ASSERT(authority.getLength() == 0 || hasAuthority);
    OSL_ASSERT(!hasQuery || isHierarchical);
    OSL_ASSERT(query.getLength() == 0 || hasQuery);
    m_scheme = scheme;
    m_isHierarchical = isHierarchical;
    m_hasAuthority = hasAuthority;
    m_authority = authority;
    m_path = path;
    m_hasQuery = hasQuery;
    m_query = query;
}

void UriReference::appendSchemeSpecificPart(rtl::OUStringBuffer & buffer) const
{
    if (m_hasAuthority) {
        buffer.appendAscii(RTL_CONSTASCII_STRINGPARAM("//"));
        buffer.append(m_authority);
    }
    buffer.append(m_path);
    if (m_hasQuery) {
        buffer.append(static_cast< sal_Unicode >('?'));
        buffer.append(m_query);
    }
}
