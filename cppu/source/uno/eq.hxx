/*************************************************************************
 *
 *  $RCSfile: eq.hxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: vg $ $Date: 2003-03-20 12:29:26 $
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
#ifndef EQ_HXX
#define EQ_HXX

#include <math.h>

#ifndef _RTL_MEMORY_H_
#include <rtl/memory.h>
#endif

#include "prim.hxx"
#include "destr.hxx"


namespace cppu
{

//##################################################################################################
//#### equality ####################################################################################
//##################################################################################################

//--------------------------------------------------------------------------------------------------
inline sal_Bool _equalObject(
    void * pI1, void * pI2,
    uno_QueryInterfaceFunc queryInterface, uno_ReleaseFunc release )
    SAL_THROW( () )
{
    if (pI1 == pI2)
        return sal_True;
    if ((0 == pI1) || (0 == pI2))
        return sal_False;
    sal_Bool bRet = sal_False;
    
    typelib_TypeDescriptionReference * type_XInterface =
        * typelib_static_type_getByTypeClass( typelib_TypeClass_INTERFACE );
    if (0 == queryInterface)
        queryInterface = binuno_queryInterface;
    pI1 = (*queryInterface)( pI1, type_XInterface );
    if (0 != pI1)
    {
        pI2 = (*queryInterface)( pI2, type_XInterface );
        if (0 != pI2)
        {
            bRet = (pI1 == pI2);
            _release( pI2, release );
        }
        _release( pI1, release );
    }
    return bRet;
}

//==================================================================================================
sal_Bool equalStruct(
    void * pDest, void *pSource,
    typelib_CompoundTypeDescription * pTypeDescr,
    uno_QueryInterfaceFunc queryInterface, uno_ReleaseFunc release )
    SAL_THROW( () );
//--------------------------------------------------------------------------------------------------
inline sal_Bool _equalStruct(
    void * pDest, void *pSource,
    typelib_CompoundTypeDescription * pTypeDescr,
    uno_QueryInterfaceFunc queryInterface, uno_ReleaseFunc release )
    SAL_THROW( () )
{
    if (pTypeDescr->pBaseTypeDescription &&
        !equalStruct( pDest, pSource, pTypeDescr->pBaseTypeDescription, queryInterface, release ))
    {
        return sal_False;
    }
    
    typelib_TypeDescriptionReference ** ppTypeRefs = pTypeDescr->ppTypeRefs;
    sal_Int32 * pMemberOffsets = pTypeDescr->pMemberOffsets;
    sal_Int32 nDescr = pTypeDescr->nMembers;
    
    while (nDescr--)
    {
        sal_Int32 nOffset = pMemberOffsets[nDescr];
        if (! ::uno_type_equalData( (char *)pDest + nOffset,
                                    ppTypeRefs[nDescr],
                                    (char *)pSource + nOffset,
                                    ppTypeRefs[nDescr],
                                    queryInterface, release ))
        {
            return sal_False;
        }
    }
    return sal_True;
}
//==================================================================================================
sal_Bool equalSequence(
    uno_Sequence * pDest, uno_Sequence * pSource,
    typelib_TypeDescriptionReference * pElementType,
    uno_QueryInterfaceFunc queryInterface, uno_ReleaseFunc release )
    SAL_THROW( () );
//--------------------------------------------------------------------------------------------------
inline sal_Bool _equalSequence(
    uno_Sequence * pDest, uno_Sequence * pSource,
    typelib_TypeDescriptionReference * pElementType,
    uno_QueryInterfaceFunc queryInterface, uno_ReleaseFunc release )
    SAL_THROW( () )
{
    if (pDest == pSource)
        return sal_True;
    sal_Int32 nElements = pDest->nElements;
    if (nElements != pSource->nElements)
        return sal_False;
    if (! nElements)
        return sal_True;
    
    void * pDestElements = pDest->elements;
    void * pSourceElements = pSource->elements;
    
    switch (pElementType->eTypeClass)
    {
    case typelib_TypeClass_CHAR:
        return (0 == ::rtl_compareMemory( pDestElements, pSourceElements, sizeof(sal_Unicode) * nElements ));
    case typelib_TypeClass_BOOLEAN:
    {
        for ( sal_Int32 nPos = nElements; nPos--; )
        {
            if ((((sal_Bool *)pDestElements)[nPos] != sal_False) !=
                (((sal_Bool *)pSourceElements)[nPos] != sal_False))
            {
                return sal_False;
            }
        }
        return sal_True;
    }
    case typelib_TypeClass_BYTE:
        return (0 == ::rtl_compareMemory( pDestElements, pSourceElements, sizeof(sal_Int8) * nElements ));
    case typelib_TypeClass_SHORT:
    case typelib_TypeClass_UNSIGNED_SHORT:
        return (0 == ::rtl_compareMemory( pDestElements, pSourceElements, sizeof(sal_Int16) * nElements ));
    case typelib_TypeClass_LONG:
    case typelib_TypeClass_UNSIGNED_LONG:
        return (0 == ::rtl_compareMemory( pDestElements, pSourceElements, sizeof(sal_Int32) * nElements ));
    case typelib_TypeClass_HYPER:
    case typelib_TypeClass_UNSIGNED_HYPER:
        return (0 == ::rtl_compareMemory( pDestElements, pSourceElements, sizeof(sal_Int64) * nElements ));
    case typelib_TypeClass_FLOAT:
    {
        for ( sal_Int32 nPos = nElements; nPos--; )
        {
            if (((float *)pDestElements)[nPos] != ((float *)pSourceElements)[nPos])
                return sal_False;
        }
        return sal_True;
    }
    case typelib_TypeClass_DOUBLE:
    {
        for ( sal_Int32 nPos = nElements; nPos--; )
        {
            if (((double *)pDestElements)[nPos] != ((double *)pSourceElements)[nPos])
                return sal_False;
        }
        return sal_True;
    }
    case typelib_TypeClass_STRING:
    {
        for ( sal_Int32 nPos = nElements; nPos--; )
        {
            if (! ((::rtl::OUString *)pDestElements +nPos)->equals( ((const ::rtl::OUString *)pSourceElements)[nPos] ))
                return sal_False;
        }
        return sal_True;
    }
    case typelib_TypeClass_TYPE:
    {
        for ( sal_Int32 nPos = nElements; nPos--; )
        {
            if (! _type_equals( ((typelib_TypeDescriptionReference **)pDestElements)[nPos],
                                ((typelib_TypeDescriptionReference **)pSourceElements)[nPos] ))
            {
                return sal_False;
            }
        }
        return sal_True;
    }
    case typelib_TypeClass_ANY:
    {
        for ( sal_Int32 nPos = nElements; nPos--; )
        {
            uno_Any * pDest = (uno_Any *)pDestElements + nPos;
            uno_Any * pSource = (uno_Any *)pSourceElements + nPos;
            if (! ::uno_type_equalData( pDest->pData, pDest->pType,
                                        pSource->pData, pSource->pType,
                                        queryInterface, release ))
            {
                return sal_False;
            }
        }
        return sal_True;
    }
    case typelib_TypeClass_ENUM:
        return (0 == ::rtl_compareMemory( pDestElements, pSourceElements, sizeof(sal_Int32) * nElements ));
    case typelib_TypeClass_STRUCT:
    case typelib_TypeClass_EXCEPTION:
    {
        typelib_TypeDescription * pElementTypeDescr = 0;
        TYPELIB_DANGER_GET( &pElementTypeDescr, pElementType );
        sal_Int32 nElementSize = pElementTypeDescr->nSize;
        for ( sal_Int32 nPos = nElements; nPos--; )
        {
            if (! _equalStruct( (char *)pDestElements + (nPos * nElementSize),
                                (char *)pSourceElements + (nPos * nElementSize),
                                (typelib_CompoundTypeDescription *)pElementTypeDescr,
                                queryInterface, release ))
            {
                TYPELIB_DANGER_RELEASE( pElementTypeDescr );
                return sal_False;
            }
        }
        TYPELIB_DANGER_RELEASE( pElementTypeDescr );
        return sal_True;
    }
    case typelib_TypeClass_UNION:
    {
        typelib_TypeDescription * pElementTypeDescr = 0;
        TYPELIB_DANGER_GET( &pElementTypeDescr, pElementType );
        sal_Int32 nElementSize = pElementTypeDescr->nSize;
        sal_Int32 nValueOffset = ((typelib_UnionTypeDescription *)pElementTypeDescr)->nValueOffset;
        for ( sal_Int32 nPos = nElements; nPos--; )
        {
            char * pDest = (char *)pDestElements + (nPos * nElementSize);
            char * pSource = (char *)pSourceElements + (nPos * nElementSize);
            typelib_TypeDescriptionReference * pSetType = _unionGetSetType(
                pDest, pElementTypeDescr );
            sal_Bool bRet = ::uno_type_equalData(
                pDest + nValueOffset, pSetType,
                pSource + nValueOffset, pSetType,
                queryInterface, release );
            ::typelib_typedescriptionreference_release( pSetType );
            if (! bRet)
            {
                TYPELIB_DANGER_RELEASE( pElementTypeDescr );
                return sal_False;
            }
        }
        TYPELIB_DANGER_RELEASE( pElementTypeDescr );
        return sal_True;
    }
    case typelib_TypeClass_SEQUENCE: // sequence of sequence
    {
        typelib_TypeDescription * pElementTypeDescr = 0;
        TYPELIB_DANGER_GET( &pElementTypeDescr, pElementType );
        typelib_TypeDescriptionReference * pSeqElementType =
            ((typelib_IndirectTypeDescription *)pElementTypeDescr)->pType;
        for ( sal_Int32 nPos = nElements; nPos--; )
        {
            if (! equalSequence( ((uno_Sequence **)pDestElements)[nPos],
                                 ((uno_Sequence **)pSourceElements)[nPos],
                                 pSeqElementType, queryInterface, release ))
            {
                TYPELIB_DANGER_RELEASE( pElementTypeDescr );
                return sal_False;
            }
        }
        TYPELIB_DANGER_RELEASE( pElementTypeDescr );
        return sal_True;
    }
    case typelib_TypeClass_INTERFACE:
    {
        for ( sal_Int32 nPos = nElements; nPos--; )
        {
            if (! _equalObject( ((void **)pDestElements)[nPos], ((void **)pSourceElements)[nPos],
                                queryInterface, release ))
            {
                return sal_False;
            }
        }
        return sal_True;
    }
    }
    return sal_False;
}
//--------------------------------------------------------------------------------------------------
inline sal_Bool _equalData(
    void * pDest,
    typelib_TypeDescriptionReference * pDestType, typelib_TypeDescription * pDestTypeDescr,
    void * pSource,
    typelib_TypeDescriptionReference * pSourceType, typelib_TypeDescription * pSourceTypeDescr,
    uno_QueryInterfaceFunc queryInterface, uno_ReleaseFunc release )
    SAL_THROW( () )
{
    typelib_TypeClass eSourceTypeClass, eDestTypeClass;
    while (typelib_TypeClass_ANY == (eDestTypeClass = pDestType->eTypeClass))
    {
        pDestTypeDescr = 0;
        pDestType = ((uno_Any *)pDest)->pType;
        pDest = ((uno_Any *)pDest)->pData;
    }
    while (typelib_TypeClass_ANY == (eSourceTypeClass = pSourceType->eTypeClass))
    {
        pSourceTypeDescr = 0;
        pSourceType = ((uno_Any *)pSource)->pType;
        pSource = ((uno_Any *)pSource)->pData;
    }
    
    switch (eDestTypeClass)
    {
    case typelib_TypeClass_VOID:
        switch (eSourceTypeClass)
        {
        case typelib_TypeClass_VOID:
            return sal_True;
        }
        return sal_False;
    case typelib_TypeClass_CHAR:
        switch (eSourceTypeClass)
        {
        case typelib_TypeClass_CHAR:
            return (*(sal_Unicode *)pDest == *(sal_Unicode *)pSource);
        }
        return sal_False;
    case typelib_TypeClass_BOOLEAN:
        switch (eSourceTypeClass)
        {
        case typelib_TypeClass_BOOLEAN:
            return ((*(sal_Bool *)pDest != sal_False) == (*(sal_Bool *)pSource != sal_False));
        }
        return sal_False;
    case typelib_TypeClass_BYTE:
        switch (eSourceTypeClass)
        {
        case typelib_TypeClass_BYTE:
            return (*(sal_Int8 *)pDest == *(sal_Int8 *)pSource);
        case typelib_TypeClass_SHORT:
            return ((sal_Int16)*(sal_Int8 *)pDest == *(sal_Int16 *)pSource);
        case typelib_TypeClass_UNSIGNED_SHORT:
            return ((sal_Int32)*(sal_Int8 *)pDest == (sal_Int32)*(sal_uInt16 *)pSource);
        case typelib_TypeClass_LONG:
            return ((sal_Int32)*(sal_Int8 *)pDest == *(sal_Int32 *)pSource);
        case typelib_TypeClass_UNSIGNED_LONG:
            return ((sal_Int64)*(sal_Int8 *)pDest == (sal_Int64)*(sal_uInt32 *)pSource);
        case typelib_TypeClass_HYPER:
            return ((sal_Int64)*(sal_Int8 *)pDest == *(sal_Int64 *)pSource);
        case typelib_TypeClass_UNSIGNED_HYPER:
            return (*(sal_Int8 *)pDest >= 0 &&
                    (sal_Int64)*(sal_Int8 *)pDest == *(sal_Int64 *)pSource); // same size
        case typelib_TypeClass_FLOAT:
            return ((float)*(sal_Int8 *)pDest == *(float *)pSource);
        case typelib_TypeClass_DOUBLE:
            return ((double)*(sal_Int8 *)pDest == *(double *)pSource);
        }
        return sal_False;
    case typelib_TypeClass_SHORT:
        switch (eSourceTypeClass)
        {
        case typelib_TypeClass_BYTE:
            return (*(sal_Int16 *)pDest == (sal_Int16)*(sal_Int8 *)pSource);
        case typelib_TypeClass_SHORT:
            return (*(sal_Int16 *)pDest == *(sal_Int16 *)pSource);
        case typelib_TypeClass_UNSIGNED_SHORT:
            return ((sal_Int32)*(sal_Int16 *)pDest == (sal_Int32)*(sal_uInt16 *)pSource);
        case typelib_TypeClass_LONG:
            return ((sal_Int32)*(sal_Int16 *)pDest == *(sal_Int32 *)pSource);
        case typelib_TypeClass_UNSIGNED_LONG:
            return ((sal_Int64)*(sal_Int16 *)pDest == (sal_Int64)*(sal_uInt32 *)pSource);
        case typelib_TypeClass_HYPER:
            return ((sal_Int64)*(sal_Int16 *)pDest == *(sal_Int64 *)pSource);
        case typelib_TypeClass_UNSIGNED_HYPER:
            return (*(sal_Int16 *)pDest >= 0 &&
                    (sal_Int64)*(sal_Int16 *)pDest == *(sal_Int64 *)pSource); // same size
        case typelib_TypeClass_FLOAT:
            return ((float)*(sal_Int16 *)pDest == *(float *)pSource);
        case typelib_TypeClass_DOUBLE:
            return ((double)*(sal_Int16 *)pDest == *(double *)pSource);
        }
        return sal_False;
    case typelib_TypeClass_UNSIGNED_SHORT:
        switch (eSourceTypeClass)
        {
        case typelib_TypeClass_BYTE:
            return ((sal_Int32)*(sal_uInt16 *)pDest == (sal_Int32)*(sal_Int8 *)pSource);
        case typelib_TypeClass_SHORT:
            return ((sal_Int32)*(sal_uInt16 *)pDest == (sal_Int32)*(sal_Int16 *)pSource);
        case typelib_TypeClass_UNSIGNED_SHORT:
            return (*(sal_uInt16 *)pDest == *(sal_uInt16 *)pSource);
        case typelib_TypeClass_LONG:
            return ((sal_Int32)*(sal_uInt16 *)pDest == *(sal_Int32 *)pSource);
        case typelib_TypeClass_UNSIGNED_LONG:
            return ((sal_uInt32)*(sal_uInt16 *)pDest == *(sal_uInt32 *)pSource);
        case typelib_TypeClass_HYPER:
            return ((sal_Int64)*(sal_uInt16 *)pDest == *(sal_Int64 *)pSource);
        case typelib_TypeClass_UNSIGNED_HYPER:
            return ((sal_uInt64)*(sal_uInt16 *)pDest == *(sal_uInt64 *)pSource);
        case typelib_TypeClass_FLOAT:
            return ((float)*(sal_uInt16 *)pDest == *(float *)pSource);
        case typelib_TypeClass_DOUBLE:
            return ((double)*(sal_uInt16 *)pDest == *(double *)pSource);
        }
        return sal_False;
    case typelib_TypeClass_LONG:
        switch (eSourceTypeClass)
        {
        case typelib_TypeClass_BYTE:
            return (*(sal_Int32 *)pDest == (sal_Int32)*(sal_Int8 *)pSource);
        case typelib_TypeClass_SHORT:
            return (*(sal_Int32 *)pDest == (sal_Int32)*(sal_Int16 *)pSource);
        case typelib_TypeClass_UNSIGNED_SHORT:
            return (*(sal_Int32 *)pDest == (sal_Int32)*(sal_uInt16 *)pSource);
        case typelib_TypeClass_LONG:
            return (*(sal_Int32 *)pDest == *(sal_Int32 *)pSource);
        case typelib_TypeClass_UNSIGNED_LONG:
            return ((sal_Int64)*(sal_Int32 *)pDest == (sal_Int64)*(sal_uInt32 *)pSource);
        case typelib_TypeClass_HYPER:
            return ((sal_Int64)*(sal_Int32 *)pDest == *(sal_Int64 *)pSource);
        case typelib_TypeClass_UNSIGNED_HYPER:
            return (*(sal_Int32 *)pDest >= 0 &&
                    (sal_Int64)*(sal_Int32 *)pDest == *(sal_Int64 *)pSource); // same size
        case typelib_TypeClass_FLOAT:
            return ((float)*(sal_Int32 *)pDest == *(float *)pSource);
        case typelib_TypeClass_DOUBLE:
            return ((double)*(sal_Int32 *)pDest == *(double *)pSource);
        }
        return sal_False;
    case typelib_TypeClass_UNSIGNED_LONG:
        switch (eSourceTypeClass)
        {
        case typelib_TypeClass_BYTE:
            return ((sal_Int64)*(sal_uInt32 *)pDest == (sal_Int64)*(sal_Int8 *)pSource);
        case typelib_TypeClass_SHORT:
            return ((sal_Int64)*(sal_uInt32 *)pDest == (sal_Int64)*(sal_Int16 *)pSource);
        case typelib_TypeClass_UNSIGNED_SHORT:
            return (*(sal_uInt32 *)pDest == (sal_uInt32)*(sal_uInt16 *)pSource);
        case typelib_TypeClass_LONG:
            return ((sal_Int64)*(sal_uInt32 *)pDest == (sal_Int64)*(sal_Int32 *)pSource);
        case typelib_TypeClass_UNSIGNED_LONG:
            return (*(sal_uInt32 *)pDest == *(sal_uInt32 *)pSource);
        case typelib_TypeClass_HYPER:
            return ((sal_Int64)*(sal_uInt32 *)pDest == *(sal_Int64 *)pSource);
        case typelib_TypeClass_UNSIGNED_HYPER:
            return ((sal_uInt64)*(sal_uInt32 *)pDest == *(sal_uInt64 *)pSource);
        case typelib_TypeClass_FLOAT:
            return ((float)*(sal_uInt32 *)pDest == *(float *)pSource);
        case typelib_TypeClass_DOUBLE:
            return ((double)*(sal_uInt32 *)pDest == *(double *)pSource);
        }
        return sal_False;
    case typelib_TypeClass_HYPER:
        switch (eSourceTypeClass)
        {
        case typelib_TypeClass_BYTE:
            return (*(sal_Int64 *)pDest == (sal_Int64)*(sal_Int8 *)pSource);
        case typelib_TypeClass_SHORT:
            return (*(sal_Int64 *)pDest == (sal_Int64)*(sal_Int16 *)pSource);
        case typelib_TypeClass_UNSIGNED_SHORT:
            return (*(sal_Int64 *)pDest == (sal_Int64)*(sal_uInt16 *)pSource);
        case typelib_TypeClass_LONG:
            return (*(sal_Int64 *)pDest == (sal_Int64)*(sal_Int32 *)pSource);
        case typelib_TypeClass_UNSIGNED_LONG:
            return (*(sal_Int64 *)pDest == (sal_Int64)*(sal_uInt32 *)pSource);
        case typelib_TypeClass_HYPER:
            return (*(sal_Int64 *)pDest == *(sal_Int64 *)pSource);
        case typelib_TypeClass_UNSIGNED_HYPER:
            return (*(sal_Int64 *)pDest >= 0 &&
                    *(sal_Int64 *)pDest == *(sal_Int64 *)pSource); // same size
        case typelib_TypeClass_FLOAT:
            return ((float)*(sal_Int64 *)pDest == *(float *)pSource);
        case typelib_TypeClass_DOUBLE:
            return ((double)*(sal_Int64 *)pDest == *(double *)pSource);
        }
        return sal_False;
    case typelib_TypeClass_UNSIGNED_HYPER:
        switch (eSourceTypeClass)
        {
        case typelib_TypeClass_BYTE:
            return (*(sal_Int8 *)pSource >= 0 &&
                    *(sal_uInt64 *)pDest == (sal_uInt64)*(sal_Int8 *)pSource);
        case typelib_TypeClass_SHORT:
            return (*(sal_Int16 *)pSource >= 0 &&
                    *(sal_uInt64 *)pDest == (sal_uInt64)*(sal_Int16 *)pSource);
        case typelib_TypeClass_UNSIGNED_SHORT:
            return (*(sal_uInt64 *)pDest == (sal_uInt64)*(sal_uInt16 *)pSource);
        case typelib_TypeClass_LONG:
            return (*(sal_Int32 *)pSource >= 0 &&
                    *(sal_uInt64 *)pDest == (sal_uInt64)*(sal_Int32 *)pSource);
        case typelib_TypeClass_UNSIGNED_LONG:
            return (*(sal_uInt64 *)pDest == (sal_uInt64)*(sal_uInt32 *)pSource);
        case typelib_TypeClass_HYPER:
            return (*(sal_Int64 *)pSource >= 0 &&
                    *(sal_uInt64 *)pDest == (sal_uInt64)*(sal_Int64 *)pSource);
        case typelib_TypeClass_UNSIGNED_HYPER:
            return (*(sal_uInt64 *)pDest == *(sal_uInt64 *)pSource);
        case typelib_TypeClass_FLOAT:
            if (::floor( *(float *)pSource ) != *(float *)pSource || *(float *)pSource < 0)
                return sal_False;
            return (*(sal_uInt64 *)pDest == (sal_uInt64)*(float *)pSource);
        case typelib_TypeClass_DOUBLE:
            if (::floor( *(double *)pSource ) != *(double *)pSource || *(double *)pSource < 0)
                return sal_False;
            return (*(sal_uInt64 *)pDest == (sal_uInt64)*(double *)pSource);
        }
        return sal_False;
    case typelib_TypeClass_FLOAT:
        switch (eSourceTypeClass)
        {
        case typelib_TypeClass_BYTE:
            return (*(float *)pDest == (float)*(sal_Int8 *)pSource);
        case typelib_TypeClass_SHORT:
            return (*(float *)pDest == (float)*(sal_Int16 *)pSource);
        case typelib_TypeClass_UNSIGNED_SHORT:
            return (*(float *)pDest == (float)*(sal_uInt16 *)pSource);
        case typelib_TypeClass_LONG:
            return (*(float *)pDest == (float)*(sal_Int32 *)pSource);
        case typelib_TypeClass_UNSIGNED_LONG:
            return (*(float *)pDest == (float)*(sal_uInt32 *)pSource);
        case typelib_TypeClass_HYPER:
            return (*(float *)pDest == (float)*(sal_Int64 *)pSource);
        case typelib_TypeClass_UNSIGNED_HYPER:
            if (::floor( *(float *)pDest ) != *(float *)pDest || *(float *)pDest < 0)
                return sal_False;
            return ((sal_uInt64)*(float *)pDest == *(sal_uInt64 *)pSource);
        case typelib_TypeClass_FLOAT:
            return (*(float *)pDest == *(float *)pSource);
        case typelib_TypeClass_DOUBLE:
            return ((double)*(float *)pDest == *(double *)pSource);
        }
        return sal_False;
    case typelib_TypeClass_DOUBLE:
        switch (eSourceTypeClass)
        {
        case typelib_TypeClass_BYTE:
            return (*(double *)pDest == (double)*(sal_Int8 *)pSource);
        case typelib_TypeClass_SHORT:
            return (*(double *)pDest == (double)*(sal_Int16 *)pSource);
        case typelib_TypeClass_UNSIGNED_SHORT:
            return (*(double *)pDest == (double)*(sal_uInt16 *)pSource);
        case typelib_TypeClass_LONG:
            return (*(double *)pDest == (double)*(sal_Int32 *)pSource);
        case typelib_TypeClass_UNSIGNED_LONG:
            return (*(double *)pDest == (double)*(sal_uInt32 *)pSource);
        case typelib_TypeClass_HYPER:
            return (*(double *)pDest == (double)*(sal_Int64 *)pSource);
        case typelib_TypeClass_UNSIGNED_HYPER:
            if (::floor( *(double *)pDest ) != *(double *)pDest || *(double *)pDest < 0)
                return sal_False;
            return ((sal_uInt64)*(double *)pDest == *(sal_uInt64 *)pSource);
        case typelib_TypeClass_FLOAT:
            return (*(double *)pDest == (double)*(float *)pSource);
        case typelib_TypeClass_DOUBLE:
            return (*(double *)pDest == *(double *)pSource);
        }
        return sal_False;
    case typelib_TypeClass_STRING:
        switch (eSourceTypeClass)
        {
        case typelib_TypeClass_STRING:
            return ((::rtl::OUString *)pDest)->equals( *(::rtl::OUString const *)pSource );
        }
        return sal_False;
    case typelib_TypeClass_TYPE:
        switch (eSourceTypeClass)
        {
        case typelib_TypeClass_TYPE:
            return _type_equals(
                *(typelib_TypeDescriptionReference **)pDest,
                *(typelib_TypeDescriptionReference **)pSource );
        }
        return sal_False;
    case typelib_TypeClass_ENUM:
        return (_type_equals( pDestType, pSourceType ) &&
                *(sal_Int32 *)pDest == *(sal_Int32 *)pSource);
    case typelib_TypeClass_TYPEDEF:
        OSL_ENSURE( 0, "### unexpected typedef!" );
        break;
    case typelib_TypeClass_STRUCT:
    case typelib_TypeClass_EXCEPTION:
        if (! _type_equals( pDestType, pSourceType ))
            return sal_False;
        if (pDestTypeDescr)
        {
            return _equalStruct(
                pDest, pSource,
                (typelib_CompoundTypeDescription *)pDestTypeDescr,
                queryInterface, release );
        }
        else
        {
            TYPELIB_DANGER_GET( &pDestTypeDescr, pDestType );
            sal_Bool bRet = _equalStruct(
                pDest, pSource,
                (typelib_CompoundTypeDescription *)pDestTypeDescr,
                queryInterface, release );
            TYPELIB_DANGER_RELEASE( pDestTypeDescr );
            return bRet;
        }
    case typelib_TypeClass_UNION:
        if (_type_equals( pDestType, pSourceType ) &&
            *(sal_Int64 *)pDest == *(sal_Int64 *)pSource) // same discriminant
        {
            sal_Bool bRet;
            if (pDestTypeDescr)
            {
                typelib_TypeDescriptionReference * pSetType = _unionGetSetType(
                    pDest, pDestTypeDescr );
                bRet = ::uno_type_equalData(
                    (char *)pDest + ((typelib_UnionTypeDescription *)pDestTypeDescr)->nValueOffset,
                    pSetType,
                    (char *)pSource + ((typelib_UnionTypeDescription *)pDestTypeDescr)->nValueOffset,
                    pSetType,
                    queryInterface, release );
                typelib_typedescriptionreference_release( pSetType );
            }
            else
            {
                TYPELIB_DANGER_GET( &pDestTypeDescr, pDestType );
                typelib_TypeDescriptionReference * pSetType = _unionGetSetType(
                    pDest, pDestTypeDescr );
                bRet = ::uno_type_equalData(
                    (char *)pDest + ((typelib_UnionTypeDescription *)pDestTypeDescr)->nValueOffset,
                    pSetType,
                    (char *)pSource + ((typelib_UnionTypeDescription *)pDestTypeDescr)->nValueOffset,
                    pSetType,
                    queryInterface, release );
                typelib_typedescriptionreference_release( pSetType );
                TYPELIB_DANGER_RELEASE( pDestTypeDescr );
            }
            return bRet;
        }
        return sal_False;
    case typelib_TypeClass_SEQUENCE:
        if (_type_equals( pDestType, pSourceType ))
        {
            if (pDestTypeDescr)
            {
                return _equalSequence(
                    *(uno_Sequence **)pDest, *(uno_Sequence **)pSource,
                    ((typelib_IndirectTypeDescription *)pDestTypeDescr)->pType,
                    queryInterface, release );
            }
            else
            {
                TYPELIB_DANGER_GET( &pDestTypeDescr, pDestType );
                sal_Bool bRet = _equalSequence(
                    *(uno_Sequence **)pDest, *(uno_Sequence **)pSource,
                    ((typelib_IndirectTypeDescription *)pDestTypeDescr)->pType,
                    queryInterface, release );
                TYPELIB_DANGER_RELEASE( pDestTypeDescr );
                return bRet;
            }
        }
        return sal_False;
    case typelib_TypeClass_INTERFACE:
        if (typelib_TypeClass_INTERFACE == eSourceTypeClass)
            return _equalObject( *(void **)pDest, *(void **)pSource, queryInterface, release );
    }
    return sal_False;
}

}

#endif
