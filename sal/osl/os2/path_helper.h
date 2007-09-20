/*************************************************************************
 *
 *  $RCSfile: path_helper.h,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: vg $ $Date: 2007-09-20 15:12:37 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *    GNU Lesser General Public License Version 2.1
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

#ifndef _PATH_HELPER_H_
#define _PATH_HELPER_H_

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#ifndef _RTL_USTRING_H_
#include <rtl/ustring.h>
#endif

#ifndef _OSL_FILE_H_
#include <osl/file.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************
 osl_systemPathEnsureSeparator
 Adds a trailing path separator to the given system path if not 
 already there and if the path is not the root path or a logical
 drive alone
 ******************************************************************/
 
void osl_systemPathEnsureSeparator(/*inout*/ rtl_uString** ppustrPath);

/*******************************************************************
 osl_systemPathRemoveSeparator
 Removes the last separator from the given system path if any and 
 if the path is not the root path '\'
 ******************************************************************/
 
void SAL_CALL osl_systemPathRemoveSeparator(/*inout*/ rtl_uString** ppustrPath);

/*******************************************************************
 osl_is_logical_drive_pattern
 Returns whether a given path is only a logical drive pattern or not.
 A logical drive pattern is something like "a:\", "c:\".
 No logical drive pattern is something like "c:\test"
 ******************************************************************/
 
sal_Int32 osl_systemPathIsLogicalDrivePattern(/*in*/ const rtl_uString* pustrPath);
          
#ifdef __cplusplus
}
#endif

#endif
