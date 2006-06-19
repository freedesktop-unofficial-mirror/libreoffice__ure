/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: prim.hxx,v $
 *
 *  $Revision: 1.17 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 13:15:15 $
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
#ifndef PRIM_HXX
#define PRIM_HXX

#ifndef _TYPELIB_TYPEDESCRIPTION_H_
#include "typelib/typedescription.h"
#endif
#ifndef _typelib_TypeClass_H_
#include "typelib/typeclass.h"
#endif
#ifndef _UNO_SEQUENCE2_H_
#include "uno/sequence2.h"
#endif
#ifndef _UNO_ANY2_H_
#include "uno/any2.h"
#endif
#ifndef _UNO_DATA_H_
#include "uno/data.h"
#endif
#ifndef _UNO_MAPPING_H_
#include "uno/mapping.h"
#endif
#ifndef _UNO_DISPATCHER_H_
#include "uno/dispatcher.h"
#endif

#ifndef _OSL_INTERLCK_H
#include "osl/interlck.h"
#endif
#ifndef _OSL_DIAGNOSE_H_
#include "osl/diagnose.h"
#endif
#ifndef _RTL_USTRING_HXX
#include "rtl/ustring.hxx"
#endif
#ifndef _RTL_ALLOC_H_
#include "rtl/alloc.h"
#endif

#if OSL_DEBUG_LEVEL > 1
#include "rtl/ustrbuf.hxx"
#include "rtl/string.hxx"
#endif


namespace cppu
{

extern uno_Sequence g_emptySeq;
extern typelib_TypeDescriptionReference * g_pVoidType;

//--------------------------------------------------------------------------------------------------
inline void * _map(
    void * p,
    typelib_TypeDescriptionReference * pType, typelib_TypeDescription * pTypeDescr,
    uno_Mapping * mapping )
    SAL_THROW( () )
{
    void * pRet = 0;
    if (p)
    {
        if (pTypeDescr)
        {
            (*mapping->mapInterface)(
                mapping, &pRet, p, (typelib_InterfaceTypeDescription *)pTypeDescr );
        }
        else
        {
            TYPELIB_DANGER_GET( &pTypeDescr, pType );
            (*mapping->mapInterface)(
                mapping, &pRet, p, (typelib_InterfaceTypeDescription *)pTypeDescr );
            TYPELIB_DANGER_RELEASE( pTypeDescr );
        }
    }
    return pRet;
}
//--------------------------------------------------------------------------------------------------
inline void _acquire( void * p, uno_AcquireFunc acquire ) SAL_THROW( () )
{
    if (p)
    {
        if (acquire)
        {
            (*acquire)( p );
        }
        else
        {
            (*((uno_Interface *)p)->acquire)( (uno_Interface *)p );
        }
    }
}
//--------------------------------------------------------------------------------------------------
inline void _release( void * p, uno_ReleaseFunc release ) SAL_THROW( () )
{
    if (p)
    {
        if (release)
        {
            (*release)( p );
        }
        else
        {
            (*((uno_Interface *)p)->release)( (uno_Interface *)p );
        }
    }
}

//------------------------------------------------------------------------------
inline sal_uInt32 calcSeqMemSize(
    sal_Int32 nElementSize, sal_Int32 nElements )
{
    sal_uInt64 nSize =
        (sal_uInt64) SAL_SEQUENCE_HEADER_SIZE +
        ((sal_uInt64) nElementSize * (sal_uInt64) nElements);
    if (nSize > 0xffffffffU)
        return 0;
    else
        return (sal_uInt32) nSize;
}

//--------------------------------------------------------------------------------------------------
inline uno_Sequence * createEmptySequence() SAL_THROW( () )
{
    ::osl_incrementInterlockedCount( &g_emptySeq.nRefCount );
    return &g_emptySeq;
}
//--------------------------------------------------------------------------------------------------
inline typelib_TypeDescriptionReference * _getVoidType()
    SAL_THROW( () )
{
    if (! g_pVoidType)
    {
        g_pVoidType = * ::typelib_static_type_getByTypeClass( typelib_TypeClass_VOID );
    }
    ::typelib_typedescriptionreference_acquire( g_pVoidType );
    return g_pVoidType;
}

//--------------------------------------------------------------------------------------------------
#if OSL_DEBUG_LEVEL > 0
#define CONSTRUCT_EMPTY_ANY( pAny ) \
(pAny)->pType = _getVoidType(); \
(pAny)->pData = (void *)0xdeadbeef;
#else
#define CONSTRUCT_EMPTY_ANY( pAny ) \
(pAny)->pType = _getVoidType(); \
(pAny)->pData = (pAny);
#endif

//--------------------------------------------------------------------------------------------------
#define TYPE_ACQUIRE( pType ) \
    ::osl_incrementInterlockedCount( &(pType)->nRefCount );

//--------------------------------------------------------------------------------------------------
extern "C" void * binuno_queryInterface(
    void * pUnoI, typelib_TypeDescriptionReference * pDestType );

//--------------------------------------------------------------------------------------------------
inline typelib_TypeDescriptionReference * _unionGetSetType(
    void * pUnion, typelib_TypeDescription * pTD )
    SAL_THROW( () )
{
    typelib_TypeDescriptionReference * pRet = 0;
    sal_Int32 nPos;

    sal_Int64 * pDiscr = ((typelib_UnionTypeDescription *)pTD)->pDiscriminants;
    sal_Int64 nDiscr   = *(sal_Int64 *)pUnion;
    for ( nPos = ((typelib_UnionTypeDescription *)pTD)->nMembers; nPos--; )
    {
        if (pDiscr[nPos] == nDiscr)
        {
            pRet = ((typelib_UnionTypeDescription *)pTD)->ppTypeRefs[nPos];
            break;
        }
    }
    if (nPos >= 0)
    {
        // default
        pRet = ((typelib_UnionTypeDescription *)pTD)->pDefaultTypeRef;
    }
    typelib_typedescriptionreference_acquire( pRet );
    return pRet;
}
//--------------------------------------------------------------------------------------------------
inline sal_Bool _type_equals(
    typelib_TypeDescriptionReference * pType1, typelib_TypeDescriptionReference * pType2 )
    SAL_THROW( () )
{
    return (pType1 == pType2 ||
            (pType1->eTypeClass == pType2->eTypeClass &&
             pType1->pTypeName->length == pType2->pTypeName->length &&
             ::rtl_ustr_compare( pType1->pTypeName->buffer, pType2->pTypeName->buffer ) == 0));
}

}

#endif
