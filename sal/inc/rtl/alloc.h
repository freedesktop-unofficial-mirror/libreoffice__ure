/*************************************************************************
 *
 *  $RCSfile: alloc.h,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: mhu $ $Date: 2001-11-29 19:02:40 $
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
 *  Contributor(s): Matthias Huetsch <matthias.huetsch@sun.com>
 *
 *
 ************************************************************************/

#ifndef _RTL_ALLOC_H_
#define _RTL_ALLOC_H_

#ifndef _SAL_TYPES_H_
#	include <sal/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


/** Allocate memory.
    @descr A call to this function will return NULL upon the requested
    memory size being either zero or larger than currently allocatable.

    @param  Bytes [in] memory size.
    @return pointer to allocated memory.
 */
void * SAL_CALL rtl_allocateMemory (
    sal_uInt32 Bytes
) SAL_THROW_EXTERN_C();


/** Reallocate memory.
    @descr A call to this function with parameter 'Ptr' being NULL
    is equivalent to a rtl_allocateMemory() call.

    A call to this function with parameter 'Bytes' being 0
    is equivalent to a rtl_freeMemory() call.

    @see rtl_allocateMemory()
    @see rtl_freeMemory()

    @param  Ptr   [in] pointer to previously allocated memory.
    @param  Bytes [in] new memory size.
    @return pointer to reallocated memory. May differ from Ptr.
 */
void * SAL_CALL rtl_reallocateMemory (
    void *     Ptr,
    sal_uInt32 Bytes
) SAL_THROW_EXTERN_C();


/** Free memory.
    @param  Ptr   [in] pointer to previously allocated memory.
    @return none. Memory is released. Ptr is invalid.
 */
void SAL_CALL rtl_freeMemory (
    void * Ptr
) SAL_THROW_EXTERN_C();


/** Allocate and zero memory.
    @descr A call to this function will return NULL upon the requested
    memory size being either zero or larger than currently allocatable.

    @param  Bytes [in] memory size.
    @return pointer to allocated and zero'ed memory.
 */
void * SAL_CALL rtl_allocateZeroMemory (
    sal_uInt32 Bytes
) SAL_THROW_EXTERN_C();


/** Zero and free memory.
    @param  Ptr   [in] pointer to previously allocated memory.
    @param  Bytes [in] memory size.
    @return none. Memory is zero'ed and released. Ptr is invalid.
 */
void SAL_CALL rtl_freeZeroMemory (
    void *     Ptr,
    sal_uInt32 Bytes
) SAL_THROW_EXTERN_C();


#ifdef __cplusplus
}
#endif

#endif /*_RTL_ALLOC_H_ */

