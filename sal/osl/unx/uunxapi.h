/*************************************************************************
 *
 *  $RCSfile: uunxapi.h,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: hr $ $Date: 2004-02-03 13:25:17 $
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
 
 #ifndef _OSL_UUNXAPI_H_
 #define _OSL_UUNXAPI_H_
  
 #ifndef _UNISTD_H
 #include <unistd.h>
 #endif
 
 #ifndef _STDLIB_H
 #include <stdlib.h>
 #endif
 
 #ifndef _TYPES_H
 #include <sys/types.h>
 #endif
 
 #ifndef _STAT_H
 #include <sys/stat.h>
 #endif
 
 #ifndef _RTL_USTRING_H_
 #include <rtl/ustring.h>
 #endif
 
 
 #ifdef __cplusplus
 extern "C"
 {
 #endif
    
 /* @see access */   
 int access_u(const rtl_uString* pustrPath, int mode);
 
 /***********************************  
  @descr
  The return value differs from the
  realpath function
  
  @returns sal_True on success else
  sal_False
  
  @see realpath
  **********************************/  
 sal_Bool realpath_u(
     const rtl_uString* pustrFileName, 
    rtl_uString** ppustrResolvedName);
   
 /* @see lstat */
 int lstat_u(const rtl_uString* pustrPath, struct stat* buf);
 
 /* @see mkdir */
 int mkdir_u(const rtl_uString* path, mode_t mode);
 
 #ifdef __cplusplus
 }
 #endif
 
 
 #endif /* _OSL_UUNXAPI_H_ */
 
