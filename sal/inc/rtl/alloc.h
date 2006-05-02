/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: alloc.h,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: rt $ $Date: 2006-05-02 12:09:02 $
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
    sal_Size Bytes
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
    void *   Ptr,
    sal_Size Bytes
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
    sal_Size Bytes
) SAL_THROW_EXTERN_C();


/** Zero and free memory.
    @param  Ptr   [in] pointer to previously allocated memory.
    @param  Bytes [in] memory size.
    @return none. Memory is zero'ed and released. Ptr is invalid.
 */
void SAL_CALL rtl_freeZeroMemory (
    void *   Ptr,
    sal_Size Bytes
) SAL_THROW_EXTERN_C();


/** Opaque rtl_arena_type.
 */
typedef struct rtl_arena_st rtl_arena_type;

#define RTL_ARENA_NAME_LENGTH 31


/** rtl_arena_create()
 *
 *  @param  pName             [in] descriptive name; for debugging purposes.
 *  @param  quantum           [in] resource allocation unit / granularity; rounded up to next power of 2.
 *  @param  quantum_cache_max [in] max resources to cache; rounded up to next multiple of quantum; usually 0.
 *  @param  source_arena      [in] passed as argument to source_alloc, source_free; usually NULL.
 *  @param  source_alloc      [in] function to allocate resources; usually rtl_arena_alloc.
 *  @param  source_free       [in] function to free resources; usually rtl_arena_free.
 *  @param  nFlags            [in] flags; usually 0.
 *
 *  @return pointer to rtl_arena_type, or NULL upon failure.
 *
 *  @see rtl_arena_destroy()
 */
rtl_arena_type *
SAL_CALL rtl_arena_create (
    const char *       pName,
    sal_Size           quantum,
    sal_Size           quantum_cache_max,
    rtl_arena_type *   source_arena,
    void * (SAL_CALL * source_alloc)(rtl_arena_type *, sal_Size *),
    void   (SAL_CALL * source_free) (rtl_arena_type *, void *, sal_Size),
    int                nFlags
) SAL_THROW_EXTERN_C();


/** rtl_arena_destroy()
 *
 *  @param  pArena [in] the arena to destroy.
 *  @return None
 *
 *  @see rtl_arena_create()
 */
void
SAL_CALL rtl_arena_destroy (
    rtl_arena_type * pArena
) SAL_THROW_EXTERN_C();


/** rtl_arena_alloc()
 *
 *  @param  pArena [in]    arena from which resource is allocated.
 *  @param  pBytes [inout] size of resource to allocate.
 *
 *  @return allocated resource, or NULL upon failure.
 *
 *  @see rtl_arena_free()
 */
void *
SAL_CALL rtl_arena_alloc (
    rtl_arena_type * pArena,
    sal_Size *       pBytes
) SAL_THROW_EXTERN_C();


/** rtl_arena_free()
 *
 *  @param  pArena [in] arena from which resource was allocated.
 *  @param  pAddr  [in] resource to free.
 *  @param  nBytes [in] size of resource.
 *
 *  @return None.
 *
 *  @see rtl_arena_alloc()
 */
void
SAL_CALL rtl_arena_free (
    rtl_arena_type * pArena,
    void *           pAddr,
    sal_Size         nBytes
) SAL_THROW_EXTERN_C();


/** Opaque rtl_cache_type.
 */
typedef struct rtl_cache_st rtl_cache_type;

#define RTL_CACHE_NAME_LENGTH 31

#define RTL_CACHE_FLAG_BULKDESTROY 1

/** rtl_cache_create()
 *
 *  @param  pName       [in] descriptive name; for debugging purposes.
 *  @param  nObjSize    [in] object size.
 *  @param  nObjAlign   [in] object alignment; usually 0 for suitable default.
 *  @param  constructor [in] object constructor callback function; returning 1 for success or 0 for failure.
 *  @param  destructor  [in] object destructor callback function.
 *  @param  reclaim     [in] reclaim callback function.
 *  @param  pUserArg    [in] opaque argument passed to callback functions.
 *  @param  nFlags      [in] flags.
 *
 *  @return pointer to rtl_cache_type, or NULL upon failure.
 *
 *  @see rtl_cache_destroy()
 */
rtl_cache_type *
SAL_CALL rtl_cache_create (
    const char *     pName,
    sal_Size         nObjSize,
    sal_Size         nObjAlign,
    int  (SAL_CALL * constructor)(void * pObj, void * pUserArg),
    void (SAL_CALL * destructor) (void * pObj, void * pUserArg),
    void (SAL_CALL * reclaim)    (void * pUserArg),
    void *           pUserArg,
    rtl_arena_type * pSource,
    int              nFlags
) SAL_THROW_EXTERN_C();


/** rtl_cache_destroy()
 *
 *  @param  pCache [in] the cache to destroy.
 *
 *  @return None.
 *
 *  @see rtl_cache_create()
 */
void
SAL_CALL rtl_cache_destroy (
    rtl_cache_type * pCache
) SAL_THROW_EXTERN_C();


/** rtl_cache_alloc()
 *
 *  @param  pCache [in] cache from which object is allocated.
 *
 *  @return pointer to allocated object, or NULL upon failure.
 */
void *
SAL_CALL rtl_cache_alloc (
    rtl_cache_type * pCache
) SAL_THROW_EXTERN_C();


/** rtl_cache_free()
 *
 *  @param  pCache [in] cache from which object was allocated.
 *  @param  pObj   [in] object to free.
 *
 *  @return None.
 *
 *  @see rtl_cache_alloc()
 */
void
SAL_CALL rtl_cache_free (
    rtl_cache_type * pCache,
    void *           pObj
) SAL_THROW_EXTERN_C();


#ifdef __cplusplus
}
#endif

#endif /*_RTL_ALLOC_H_ */

