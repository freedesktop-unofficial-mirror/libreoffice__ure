/*************************************************************************
 *
 *  $RCSfile: tdsingleton.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: kz $ $Date: 2004-03-25 14:49:19 $
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

#ifndef _STOC_RDBTDP_BASE_HXX
#include "base.hxx"
#endif

using namespace com::sun::star;

namespace stoc_rdbtdp
{

//__________________________________________________________________________________________________
// virtual
SingletonTypeDescriptionImpl::~SingletonTypeDescriptionImpl()
{
    delete _pServiceTD;

    g_moduleCount.modCnt.release( &g_moduleCount.modCnt );
}

// XTypeDescription
//__________________________________________________________________________________________________
// virtual
TypeClass SingletonTypeDescriptionImpl::getTypeClass()
    throw(::com::sun::star::uno::RuntimeException)
{
    return TypeClass_SINGLETON;
}
//__________________________________________________________________________________________________
// virtual
OUString SingletonTypeDescriptionImpl::getName()
    throw(::com::sun::star::uno::RuntimeException)
{
    return _aName;
}

// XSingletonTypeDescription
//__________________________________________________________________________________________________
// virtual
Reference< XServiceTypeDescription > SAL_CALL
SingletonTypeDescriptionImpl::getService()
    throw(::com::sun::star::uno::RuntimeException)
{
    if ( !_pServiceTD )
    {
        Reference< XServiceTypeDescription > * pServiceTD
            = new Reference< XServiceTypeDescription >();
        try
        {
            _xTDMgr->getByHierarchicalName( _aServiceName ) >>= *pServiceTD;
        }
        catch ( NoSuchElementException const & )
        {
        }
        OSL_ENSURE( (*pServiceTD).is(), "### no type description for service!" );

        ClearableMutexGuard aGuard( getMutex() );
        if ( _pServiceTD )
        {
            aGuard.clear();
            delete pServiceTD;
        }
        else
        {
            _pServiceTD = pServiceTD;
        }
    }

    return *_pServiceTD;
}

}



