/*************************************************************************
 *
 *  $RCSfile: remote.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: jl $ $Date: 2001-03-12 14:37:21 $
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
#ifndef _BRIDGES_REMOTE_REMOTE_HXX_
#define _BRIDGES_REMOTE_REMOTE_HXX_
#include <list>

#include <osl/mutex.hxx>
#include <osl/conditn.h>
#include <osl/interlck.h>

//#include <vos/thread.hxx>

#include <uno/environment.h>

#include <bridges/remote/remote.h>
#include <bridges/remote/connection.h>

#include <com/sun/star/uno/Sequence.hxx>

namespace bridges_remote {

class Remote2RemoteStub :
    public remote_Interface
{
public:
    Remote2RemoteStub(rtl_uString *pOid,
                      typelib_InterfaceTypeDescription *pType,
                      uno_Environment *pEnvRemote,
                      requestClientSideDispatcher dispatch );
    ~Remote2RemoteStub();

    static void SAL_CALL thisAcquire( remote_Interface *pThis );
    static void SAL_CALL thisRelease( remote_Interface *pThis );
    static void SAL_CALL thisDispatch( remote_Interface * pUnoI, 
                                       typelib_TypeDescription * pMemberType,
                                       void * pReturn,
                                       void * pArgs[],
                                       uno_Any ** ppException );
    static void SAL_CALL thisFree( uno_ExtEnvironment *pEnvRemote , void * );
    
    void releaseRemote();
public:
    ::rtl::OUString                   m_sOid;
    typelib_InterfaceTypeDescription  *m_pType;
    oslInterlockedCount               m_nRef;
    uno_Environment                   *m_pEnvRemote;
    requestClientSideDispatcher       m_dispatch;
    oslInterlockedCount m_nReleaseRemote;
};

}
#endif
