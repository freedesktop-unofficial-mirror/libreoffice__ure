/*************************************************************************
 *
 *  $RCSfile: dllentry.c,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: vg $ $Date: 2007-09-20 15:09:28 $
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

#include "system.h"
#include "sockimpl.h"
#include "secimpl.h"
//#include "daemimpl.h"

#include <osl/diagnose.h>

#ifndef GCC
sal_uInt32 _System _DLL_InitTerm( sal_uInt32 nModule,
                                     sal_uInt32 nFlag )
{
    switch( nFlag )
    {
        case 0:
        {
            /* initialize C runtime library */
            _CRT_init();
            {
                LONG fhToAdd = 0;
                ULONG fhOld = 0;
                ULONG ngLastError = DosSetRelMaxFH(&fhToAdd, &fhOld);
                if (fhOld < 200)
                    ngLastError = DosSetMaxFH(200);
            }

            /* turn off hardware-errors and exception popups */
            DosError(FERR_DISABLEHARDERR | FERR_DISABLEEXCEPTION);

            break;
        }

        case 1:
        {
            /* unload libs (sockets) */
            ImplFreeTCPIP();

            /* unload libs (security) */
            ImplFreeUPM();

            break;
        }
    }

    return (sal_True);
}
#endif


