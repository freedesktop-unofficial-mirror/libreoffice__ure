/*************************************************************************
 *
 *  $RCSfile: UriReference.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2004-03-19 13:21:17 $
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

#ifndef INCLUDED_stoc_source_uriproc_UriReference_hxx
#define INCLUDED_stoc_source_uriproc_UriReference_hxx

#include "com/sun/star/uno/RuntimeException.hpp"
#include "osl/mutex.hxx"
#include "rtl/ustring.hxx"
#include "sal/types.h"

namespace rtl { class OUStringBuffer; }

namespace stoc { namespace uriproc {

class UriReference {
public:
    UriReference(
        rtl::OUString const & scheme, bool isHierarchical, bool hasAuthority,
        rtl::OUString const & authority, rtl::OUString const & path,
        bool hasQuery, rtl::OUString const & query);

    ~UriReference();

    rtl::OUString getUriReference()
        throw (com::sun::star::uno::RuntimeException);

    sal_Bool isAbsolute() throw (com::sun::star::uno::RuntimeException);

    rtl::OUString getScheme() throw (com::sun::star::uno::RuntimeException);

    rtl::OUString getSchemeSpecificPart()
        throw (com::sun::star::uno::RuntimeException);

    sal_Bool isHierarchical() throw (com::sun::star::uno::RuntimeException);

    sal_Bool hasAuthority() throw (com::sun::star::uno::RuntimeException);

    rtl::OUString getAuthority() throw (com::sun::star::uno::RuntimeException);

    rtl::OUString getPath() throw (com::sun::star::uno::RuntimeException);

    sal_Bool hasRelativePath() throw (com::sun::star::uno::RuntimeException);

    sal_Int32 getPathSegmentCount()
        throw (com::sun::star::uno::RuntimeException);

    rtl::OUString getPathSegment(sal_Int32 index)
        throw (com::sun::star::uno::RuntimeException);

    sal_Bool hasQuery() throw (com::sun::star::uno::RuntimeException);

    rtl::OUString getQuery() throw (com::sun::star::uno::RuntimeException);

    sal_Bool hasFragment() throw (com::sun::star::uno::RuntimeException);

    rtl::OUString getFragment() throw (com::sun::star::uno::RuntimeException);

    void setFragment(rtl::OUString const & fragment)
        throw (com::sun::star::uno::RuntimeException);

    void clearFragment() throw (com::sun::star::uno::RuntimeException);

    osl::Mutex m_mutex;
    rtl::OUString m_scheme;
    rtl::OUString m_authority;
    rtl::OUString m_path;
    rtl::OUString m_query;
    rtl::OUString m_fragment;
    bool m_isHierarchical;
    bool m_hasAuthority;
    bool m_hasQuery;
    bool m_hasFragment;

private:
    UriReference(UriReference &); // not implemented
    void operator =(UriReference); // not implemented

    void appendSchemeSpecificPart(rtl::OUStringBuffer & buffer) const;
};

} }

#endif
