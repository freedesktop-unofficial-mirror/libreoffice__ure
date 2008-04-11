/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: td.cxx,v $
 * $Revision: 1.5 $
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
#include "precompiled_stoc.hxx"

#include "osl/doublecheckedlocking.h"
#include "base.hxx"

namespace stoc_rdbtdp
{

//------------------------------------------------------------------------------
::osl::Mutex & getMutex()
{
    static ::osl::Mutex * s_pmutex = 0;
    if (s_pmutex == 0)
    {
        ::osl::MutexGuard guard( ::osl::Mutex::getGlobalMutex() );
        if (s_pmutex == 0)
        {
            static ::osl::Mutex s_mutex;
            OSL_DOUBLE_CHECKED_LOCKING_MEMORY_BARRIER();
            s_pmutex = &s_mutex;
        }
    }
    else
    {
        OSL_DOUBLE_CHECKED_LOCKING_MEMORY_BARRIER();
    }
    return *s_pmutex;
}


TypeDescriptionImpl::~TypeDescriptionImpl()
{
    g_moduleCount.modCnt.release( &g_moduleCount.modCnt );
}
    
// XTypeDescription
//__________________________________________________________________________________________________
TypeClass TypeDescriptionImpl::getTypeClass()
    throw(::com::sun::star::uno::RuntimeException)
{
    return _eTypeClass;
}
//__________________________________________________________________________________________________
OUString TypeDescriptionImpl::getName()
    throw(::com::sun::star::uno::RuntimeException)
{
    return _aName;
}

}


