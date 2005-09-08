/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: process.h,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 14:40:19 $
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
#ifndef _RTL_PROCESS_H_
#define _RTL_PROCESS_H_

#include <sal/types.h>
#include <osl/process.h>

#ifdef __cplusplus
extern "C" {
#endif	


/**
    gets a 16-byte fixed size identifier which is guaranteed not to change
    during the current process. 

    The current implementation creates a 16-byte uuid without using
    the ethernet address of system. Thus the
    identifier is different from identifiers created
    in other processes with a very probability.

    @param pTargetUUID 16 byte of memory
    @see rtl_createUiid()
 */
void SAL_CALL rtl_getGlobalProcessId( sal_uInt8 *pTargetUUID );

/** Get the nArg-th command-line argument passed to the main-function of this process.

    This functions differs from osl_getCommandArg() in filtering any bootstrap values
    given by command args, that means that all arguments starting with "-env:" will be
    ignored by this function.
    
    @param nArg [in] The number of the argument to return.
    @param strCommandArg [out] The string receives the nArg-th command-line argument.
    @return osl_Process_E_None or does not return. 	
    @see osl_getCommandArg()
    @see rtl_getCommandArgCount()
*/
oslProcessError SAL_CALL rtl_getAppCommandArg(sal_uInt32 nArg, rtl_uString **strCommandArg);

/** Returns the number of command line arguments at process start.
    
    This functions differs from osl_getCommandArg() in filtering any bootstrap values
    given by command args, that means that all arguments starting with "-env:" will be
    ignored by this function.

    @return the number of commandline arguments passed to the main-function of this process.
    @see osl_getCommandArgCount()
    @see rtl_getCommandArg()
*/
sal_uInt32 SAL_CALL rtl_getAppCommandArgCount();

#ifdef __cplusplus
}
#endif

#endif
