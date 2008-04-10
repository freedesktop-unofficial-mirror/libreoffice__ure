/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: rtl_process.c,v $
 * $Revision: 1.4 $
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
#include <string.h>
#include <osl/mutex.h>
#include <rtl/uuid.h>

/* rtl_getCommandArg, rtl_getCommandArgCount see cmdargs.cxx  */

void SAL_CALL rtl_getGlobalProcessId( sal_uInt8 *pTargetUUID )
{
    static sal_uInt8 *pUuid = 0;
    if( ! pUuid )
    {
        osl_acquireMutex( * osl_getGlobalMutex() );
        if( ! pUuid )
        {
            static sal_uInt8 aUuid[16];
            rtl_createUuid( aUuid , 0 , sal_False );
            pUuid = aUuid;
        }
        osl_releaseMutex( * osl_getGlobalMutex() );
    }
    memcpy( pTargetUUID , pUuid , 16 );
}

