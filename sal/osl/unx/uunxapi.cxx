/*************************************************************************
 *
 *  $RCSfile: uunxapi.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: hr $ $Date: 2004-02-03 13:24:49 $
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
 #include "uunxapi.h"
 #endif 
 
 #ifndef _LIMITS_H
 #include <limits.h>
 #endif
  
 #ifndef _RTL_USTRING_HXX_
 #include <rtl/ustring.hxx>
 #endif
 
 #ifndef _OSL_THREAD_H_
 #include <osl/thread.h>
 #endif
 
 //###########################
 inline rtl::OString OUStringToOString(const rtl_uString* s)
 {
    return rtl::OUStringToOString(
        rtl::OUString(const_cast<rtl_uString*>(s)),
        osl_getThreadTextEncoding());
 }
 
 //###########################
 //access_u     
 int access_u(const rtl_uString* pustrPath, int mode)
 { 		
    return access(OUStringToOString(pustrPath).getStr(), mode);
 }
 
 //#########################
 //realpath_u  
 sal_Bool realpath_u(const rtl_uString* pustrFileName, rtl_uString** ppustrResolvedName)
 {
     rtl::OString fn = rtl::OUStringToOString(
        rtl::OUString(const_cast<rtl_uString*>(pustrFileName)),
        osl_getThreadTextEncoding());
        
    char  rp[PATH_MAX];
    bool  bRet = realpath(fn.getStr(), rp); 
    
    if (bRet)
    {
        rtl::OUString resolved = rtl::OStringToOUString(
            rtl::OString(static_cast<sal_Char*>(rp)),
            osl_getThreadTextEncoding());
            
        rtl_uString_assign(ppustrResolvedName, resolved.pData);
    }
    return bRet;
 }
 
 //#########################
 //lstat_u 
  int lstat_u(const rtl_uString* pustrPath, struct stat* buf)
 { 	
    return lstat(OUStringToOString(pustrPath).getStr(), buf);
 } 
 
 //#########################
 // @see mkdir
 int mkdir_u(const rtl_uString* path, mode_t mode)
 {    
    return mkdir(OUStringToOString(path).getStr(), mode);     
 }
 
