/*************************************************************************
 *
 *  $RCSfile: urp_dispatch.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: jbu $ $Date: 2000-09-29 08:42:06 $
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

#include <rtl/ustring.hxx>

#include <typelib/typedescription.h>

#include <uno/any2.h>


typedef struct _uno_Environment uno_Environment;
struct remote_Interface;



namespace bridges_urp {

    const sal_uInt8 HDRFLAG_LONGHEADER       = 0x80;
    const sal_uInt8 HDRFLAG_REQUEST          = 0x40;
    const sal_uInt8 HDRFLAG_NEWTYPE          = 0x20;
    const sal_uInt8 HDRFLAG_NEWOID           = 0x10;
    const sal_uInt8 HDRFLAG_NEWTID           = 0x08;
    const sal_uInt8 HDRFLAG_LONGMETHODID     = 0x04;
    const sal_uInt8 HDRFLAG_IGNORECACHE      = 0x02;
    const sal_uInt8 HDRFLAG_MOREFLAGS        = 0x01;
    const sal_uInt8 HDRFLAG_MUSTREPLY        = 0x80;
    const sal_uInt8 HDRFLAG_SYNCHRONOUS      = 0x40;

    const sal_uInt8 HDRFLAG_EXCEPTION        = 0x20;

    void SAL_CALL urp_sendCloseConnection( uno_Environment *pEnvRemote );
    
    void SAL_CALL urp_sendRequest(
        uno_Environment *pEnvRemote,
        typelib_TypeDescription * pMemberType,
        rtl_uString *pOid,
        typelib_InterfaceTypeDescription *pInterfaceType,
        void *pReturn,
        void *ppArgs[],
        uno_Any **ppException
        );

}

