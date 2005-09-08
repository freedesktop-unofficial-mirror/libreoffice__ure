/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: tdmgr.cxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 09:29:44 $
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

#ifndef INCLUDED_SAL_ALLOCA_H
#include <sal/alloca.h>
#endif

#include <osl/diagnose.h>
#include <rtl/alloc.h>
#include <rtl/ustring.hxx>

#include <cppuhelper/implbase1.hxx>
#include <typelib/typedescription.h>

#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/container/XHierarchicalNameAccess.hpp>
#include <com/sun/star/reflection/XTypeDescription.hpp>
#include <com/sun/star/reflection/XEnumTypeDescription.hpp>
#include <com/sun/star/reflection/XIndirectTypeDescription.hpp>
#include <com/sun/star/reflection/XInterfaceMemberTypeDescription.hpp>
#include <com/sun/star/reflection/XInterfaceAttributeTypeDescription.hpp>
#include <com/sun/star/reflection/XMethodParameter.hpp>
#include <com/sun/star/reflection/XInterfaceMethodTypeDescription.hpp>
#include <com/sun/star/reflection/XInterfaceTypeDescription2.hpp>
#include <com/sun/star/reflection/XCompoundTypeDescription.hpp>
#include <com/sun/star/reflection/XStructTypeDescription.hpp>
#include <com/sun/star/reflection/XUnionTypeDescription.hpp>
#include "com/sun/star/uno/RuntimeException.hpp"

#include "boost/scoped_array.hpp"

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::reflection;


namespace cppu
{

static typelib_TypeDescription * createCTD(
    Reference< container::XHierarchicalNameAccess > const & access,
    const Reference< XTypeDescription > & xType );

//==================================================================================================
inline static sal_Int64 coerceToInt64( const Any & rVal )
{
    switch (rVal.getValueTypeClass())
    {
    case TypeClass_CHAR:
        return *(sal_Unicode *)rVal.getValue();
    case TypeClass_BOOLEAN:
        return (*(sal_Bool *)rVal.getValue() ? 1 : 0);
    case TypeClass_BYTE:
        return *(sal_Int8 *)rVal.getValue();
    case TypeClass_SHORT:
        return *(sal_Int16 *)rVal.getValue();
    case TypeClass_UNSIGNED_SHORT:
        return *(sal_uInt16 *)rVal.getValue();
    case TypeClass_LONG:
        return *(sal_Int32 *)rVal.getValue();
    case TypeClass_UNSIGNED_LONG:
        return *(sal_uInt32 *)rVal.getValue();
    case TypeClass_HYPER:
        return *(sal_Int64 *)rVal.getValue();
    case TypeClass_UNSIGNED_HYPER:
        return *(sal_uInt64 *)rVal.getValue();
    case TypeClass_ENUM:
        return *(int *)rVal.getValue();
    }
    OSL_ENSURE( sal_False, "### cannot read union discriminant: no ordinal type!" );
    return 0; // dummy
}
//==================================================================================================
inline static typelib_TypeDescription * createCTD(
    const Reference< XUnionTypeDescription > & xType )
{
    typelib_TypeDescription * pRet = 0;
    if (xType.is())
    {
        OUString aTypeName( xType->getName() );

        // discriminant type
        Reference< XTypeDescription > xDiscrTD( xType->getDiscriminantType() );
        OUString aDiscrTypeName( xDiscrTD->getName() );
        typelib_TypeDescriptionReference * pDiscrTypeRef = 0;
        typelib_typedescriptionreference_new( &pDiscrTypeRef,
                                              (typelib_TypeClass)xDiscrTD->getTypeClass(),
                                              aDiscrTypeName.pData );
        // default member type
        Reference< XTypeDescription > xDefaultMemberTD( xType->getDefaultMemberType() );
        OUString aDefMemberTypeName( xDefaultMemberTD->getName() );
        typelib_TypeDescriptionReference * pDefMemberTypeRef = 0;
        typelib_typedescriptionreference_new( &pDefMemberTypeRef,
                                              (typelib_TypeClass)xDefaultMemberTD->getTypeClass(),
                                              aDefMemberTypeName.pData );
        // init array
        Sequence< Any > aDiscriminants( xType->getDiscriminants() );
        Sequence< Reference< XTypeDescription > > aMemberTypes( xType->getMemberTypes() );
        Sequence< OUString > aMemberNames( xType->getMemberNames() );
        sal_Int32 nMembers = aDiscriminants.getLength();
        OSL_ASSERT( nMembers == aMemberNames.getLength() && nMembers == aMemberTypes.getLength() );

        const Any * pDiscriminants							= aDiscriminants.getConstArray();
        const Reference< XTypeDescription > * pMemberTypes	= aMemberTypes.getConstArray();
        const OUString * pMemberNames						= aMemberNames.getConstArray();

        typelib_Union_Init * pMembers = (typelib_Union_Init *)alloca( nMembers * sizeof(typelib_Union_Init) );

        sal_Int32 nPos;
        for ( nPos = nMembers; nPos--; )
        {
            typelib_Union_Init & rEntry = pMembers[nPos];
            // member discriminant
            rEntry.nDiscriminant = coerceToInt64( pDiscriminants[nPos] );
            // member type
            OUString aMemberTypeName( pMemberTypes[nPos]->getName() );
            rEntry.pTypeRef = 0;
            typelib_typedescriptionreference_new( &rEntry.pTypeRef,
                                                  (typelib_TypeClass)pMemberTypes[nPos]->getTypeClass(),
                                                  aMemberTypeName.pData );
            // member name
            rEntry.pMemberName = pMemberNames[nPos].pData;
        }

        typelib_typedescription_newUnion( &pRet, aTypeName.pData,
                                          pDiscrTypeRef,
                                          coerceToInt64( xType->getDefaultDiscriminant() ),
                                          pDefMemberTypeRef,
                                          nMembers, pMembers );

        for ( nPos = nMembers; nPos--; )
        {
            typelib_typedescriptionreference_release( pMembers[nPos].pTypeRef );
        }

        typelib_typedescriptionreference_release( pDiscrTypeRef );
        typelib_typedescriptionreference_release( pDefMemberTypeRef );
    }
    return pRet;
}
//==================================================================================================
inline static typelib_TypeDescription * createCTD(
    const Reference< XCompoundTypeDescription > & xType )
{
    typelib_TypeDescription * pRet = 0;
    if (xType.is())
    {
        typelib_TypeDescription * pBaseType = createCTD(
            Reference< XCompoundTypeDescription >::query( xType->getBaseType() ) );
        if (pBaseType)
            typelib_typedescription_register( &pBaseType );

        // construct member init array
        const Sequence<Reference< XTypeDescription > > & rMemberTypes = xType->getMemberTypes();
        const Sequence< OUString > & rMemberNames					  = xType->getMemberNames();

        const Reference< XTypeDescription > * pMemberTypes = rMemberTypes.getConstArray();
        const OUString * pMemberNames					   = rMemberNames.getConstArray();

        sal_Int32 nMembers = rMemberTypes.getLength();
        OSL_ENSURE( nMembers == rMemberNames.getLength(), "### lens differ!" );

        OUString aTypeName( xType->getName() );

        typelib_CompoundMember_Init * pMemberInits = (typelib_CompoundMember_Init *)alloca(
            sizeof(typelib_CompoundMember_Init) * nMembers );

        sal_Int32 nPos;
        for ( nPos = nMembers; nPos--; )
        {
            typelib_CompoundMember_Init & rInit = pMemberInits[nPos];
            rInit.eTypeClass = (typelib_TypeClass)pMemberTypes[nPos]->getTypeClass();

            OUString aMemberTypeName( pMemberTypes[nPos]->getName() );
            rtl_uString_acquire( rInit.pTypeName = aMemberTypeName.pData );

            // string is held by rMemberNames
            rInit.pMemberName = pMemberNames[nPos].pData;
        }

        typelib_typedescription_new(
            &pRet,
            (typelib_TypeClass)xType->getTypeClass(),
            aTypeName.pData,
            (pBaseType ? pBaseType->pWeakRef : 0),
            nMembers, pMemberInits );

        // cleanup
        for ( nPos = nMembers; nPos--; )
        {
            rtl_uString_release( pMemberInits[nPos].pTypeName );
        }
        if (pBaseType)
            typelib_typedescription_release( pBaseType );
    }
    return pRet;
}
//==================================================================================================
inline static typelib_TypeDescription * createCTD(
    Reference< container::XHierarchicalNameAccess > const & access,
    const Reference< XStructTypeDescription > & xType )
{
    typelib_TypeDescription * pRet = 0;
    if (xType.is() && xType->getTypeParameters().getLength() == 0)
    {
        typelib_TypeDescription * pBaseType = createCTD(
            access, xType->getBaseType() );
        if (pBaseType)
            typelib_typedescription_register( &pBaseType );

        // construct member init array
        const Sequence<Reference< XTypeDescription > > & rMemberTypes = xType->getMemberTypes();
        const Sequence< OUString > & rMemberNames					  = xType->getMemberNames();

        const Reference< XTypeDescription > * pMemberTypes = rMemberTypes.getConstArray();
        const OUString * pMemberNames					   = rMemberNames.getConstArray();

        sal_Int32 nMembers = rMemberTypes.getLength();
        OSL_ENSURE( nMembers == rMemberNames.getLength(), "### lens differ!" );

        OUString aTypeName( xType->getName() );

        typelib_StructMember_Init * pMemberInits = (typelib_StructMember_Init *)alloca(
            sizeof(typelib_StructMember_Init) * nMembers );

        Sequence< Reference< XTypeDescription > > templateMemberTypes;
        sal_Int32 i = aTypeName.indexOf('<');
        if (i >= 0) {
            Reference< XStructTypeDescription > templateDesc(
                access->getByHierarchicalName(aTypeName.copy(0, i)),
                UNO_QUERY_THROW);
            OSL_ASSERT(
                templateDesc->getTypeParameters().getLength()
                == xType->getTypeArguments().getLength());
            templateMemberTypes = templateDesc->getMemberTypes();
            OSL_ASSERT(templateMemberTypes.getLength() == nMembers);
        }

        sal_Int32 nPos;
        for ( nPos = nMembers; nPos--; )
        {
            typelib_StructMember_Init & rInit = pMemberInits[nPos];
            rInit.aBase.eTypeClass
                = (typelib_TypeClass)pMemberTypes[nPos]->getTypeClass();

            OUString aMemberTypeName( pMemberTypes[nPos]->getName() );
            rtl_uString_acquire(
                rInit.aBase.pTypeName = aMemberTypeName.pData );

            // string is held by rMemberNames
            rInit.aBase.pMemberName = pMemberNames[nPos].pData;

            rInit.bParameterizedType = templateMemberTypes.getLength() != 0
                && (templateMemberTypes[nPos]->getTypeClass()
                    == TypeClass_UNKNOWN);
        }

        typelib_typedescription_newStruct(
            &pRet,
            aTypeName.pData,
            (pBaseType ? pBaseType->pWeakRef : 0),
            nMembers, pMemberInits );

        // cleanup
        for ( nPos = nMembers; nPos--; )
        {
            rtl_uString_release( pMemberInits[nPos].aBase.pTypeName );
        }
        if (pBaseType)
            typelib_typedescription_release( pBaseType );
    }
    return pRet;
}
//==================================================================================================
inline static typelib_TypeDescription * createCTD(
    const Reference< XInterfaceAttributeTypeDescription > & xAttribute )
{
    typelib_TypeDescription * pRet = 0;
    if (xAttribute.is())
    {
        OUString aMemberName( xAttribute->getName() );
        Reference< XTypeDescription > xType( xAttribute->getType() );
        OUString aMemberTypeName( xType->getName() );

        typelib_typedescription_newInterfaceAttribute(
            (typelib_InterfaceAttributeTypeDescription **)&pRet,
            xAttribute->getPosition(),
            aMemberName.pData, // name
            (typelib_TypeClass)xType->getTypeClass(),
            aMemberTypeName.pData, // type name
            xAttribute->isReadOnly() );
    }
    return pRet;
}
//==================================================================================================
static typelib_TypeDescription * createCTD(
    const Reference< XInterfaceMethodTypeDescription > & xMethod )
{
    typelib_TypeDescription * pRet = 0;
    if (xMethod.is())
    {
        Reference< XTypeDescription > xReturnType( xMethod->getReturnType() );

        // init all params
        const Sequence<Reference< XMethodParameter > > & rParams = xMethod->getParameters();
        const Reference< XMethodParameter > * pParams			 = rParams.getConstArray();
        sal_Int32 nParams = rParams.getLength();

        typelib_Parameter_Init * pParamInit = (typelib_Parameter_Init *)alloca(
            sizeof(typelib_Parameter_Init) * nParams );

        sal_Int32 nPos;
        for ( nPos = nParams; nPos--; )
        {
            const Reference< XMethodParameter > & xParam = pParams[nPos];
            const Reference< XTypeDescription > & xType	 = xParam->getType();
            typelib_Parameter_Init & rInit = pParamInit[xParam->getPosition()];

            rInit.eTypeClass = (typelib_TypeClass)xType->getTypeClass();
            OUString aParamTypeName( xType->getName() );
            rtl_uString_acquire( rInit.pTypeName = aParamTypeName.pData );
            OUString aParamName( xParam->getName() );
            rtl_uString_acquire( rInit.pParamName = aParamName.pData );
            rInit.bIn  = xParam->isIn();
            rInit.bOut = xParam->isOut();
        }

        // init all exception strings
        const Sequence<Reference< XTypeDescription > > & rExceptions = xMethod->getExceptions();
        const Reference< XTypeDescription > * pExceptions = rExceptions.getConstArray();
        sal_Int32 nExceptions = rExceptions.getLength();
        rtl_uString ** ppExceptionNames = (rtl_uString **)alloca(
            sizeof(rtl_uString *) * nExceptions );

        for ( nPos = nExceptions; nPos--; )
        {
            OUString aExceptionTypeName( pExceptions[nPos]->getName() );
            rtl_uString_acquire( ppExceptionNames[nPos] = aExceptionTypeName.pData );
        }

        OUString aTypeName( xMethod->getName() );
        OUString aReturnTypeName( xReturnType->getName() );

        typelib_typedescription_newInterfaceMethod(
            (typelib_InterfaceMethodTypeDescription **)&pRet,
            xMethod->getPosition(),
            xMethod->isOneway(),
            aTypeName.pData,
            (typelib_TypeClass)xReturnType->getTypeClass(),
            aReturnTypeName.pData,
            nParams, pParamInit,
            nExceptions, ppExceptionNames );

        for ( nPos = nParams; nPos--; )
        {
            rtl_uString_release( pParamInit[nPos].pTypeName );
            rtl_uString_release( pParamInit[nPos].pParamName );
        }
        for ( nPos = nExceptions; nPos--; )
        {
            rtl_uString_release( ppExceptionNames[nPos] );
        }
    }
    return pRet;
}
//==================================================================================================
inline static typelib_TypeDescription * createCTD(
    Reference< container::XHierarchicalNameAccess > const & access,
    const Reference< XInterfaceTypeDescription2 > & xType )
{
    typelib_TypeDescription * pRet = 0;
    if (xType.is())
    {
        Sequence< Reference< XTypeDescription > > aBases(xType->getBaseTypes());
        sal_Int32 nBases = aBases.getLength();
        // Exploit the fact that a typelib_TypeDescription for an interface type
        // is also the typelib_TypeDescriptionReference for that type:
        boost::scoped_array< typelib_TypeDescription * > aBaseTypes(
            new typelib_TypeDescription *[nBases]);
        {for (sal_Int32 i = 0; i < nBases; ++i) {
            typelib_TypeDescription * p = createCTD(access, aBases[i]);
            OSL_ASSERT(
                !TYPELIB_TYPEDESCRIPTIONREFERENCE_ISREALLYWEAK(p->eTypeClass));
            typelib_typedescription_register(&p);
            aBaseTypes[i] = p;
        }}
        typelib_TypeDescriptionReference ** pBaseTypeRefs
            = reinterpret_cast< typelib_TypeDescriptionReference ** >(
                aBaseTypes.get());

        // construct all member refs
        const Sequence<Reference< XInterfaceMemberTypeDescription > > & rMembers = xType->getMembers();
        sal_Int32 nMembers = rMembers.getLength();

        typelib_TypeDescriptionReference ** ppMemberRefs = (typelib_TypeDescriptionReference **)alloca(
            sizeof(typelib_TypeDescriptionReference *) * nMembers );

        const Reference< XInterfaceMemberTypeDescription > * pMembers = rMembers.getConstArray();

        OUString aTypeName( xType->getName() );

        sal_Int32 nPos;
        for ( nPos = nMembers; nPos--; )
        {
            OUString aMemberTypeName( pMembers[nPos]->getName() );
            ppMemberRefs[nPos] = 0;
            typelib_typedescriptionreference_new(
                ppMemberRefs + nPos,
                (typelib_TypeClass)pMembers[nPos]->getTypeClass(),
                aMemberTypeName.pData );
        }

        Uik uik = xType->getUik();

        typelib_typedescription_newMIInterface(
            (typelib_InterfaceTypeDescription **)&pRet,
            aTypeName.pData,
            uik.m_Data1, uik.m_Data2, uik.m_Data3, uik.m_Data4, uik.m_Data5,
            nBases, pBaseTypeRefs,
            nMembers, ppMemberRefs );

        // cleanup refs and base type
        {for (int i = 0; i < nBases; ++i) {
            typelib_typedescription_release(aBaseTypes[i]);
        }}

        for ( nPos = nMembers; nPos--; )
        {
            typelib_typedescriptionreference_release( ppMemberRefs[nPos] );
        }
    }
    return pRet;
}
//==================================================================================================
inline static typelib_TypeDescription * createCTD( const Reference< XEnumTypeDescription > & xType )
{
    typelib_TypeDescription * pRet = 0;
    if (xType.is())
    {
        OUString aTypeName( xType->getName() );
        Sequence< OUString > aNames( xType->getEnumNames() );
        OSL_ASSERT( sizeof(OUString) == sizeof(rtl_uString *) ); // !!!
        Sequence< sal_Int32 > aValues( xType->getEnumValues() );

        typelib_typedescription_newEnum(
            &pRet, aTypeName.pData, xType->getDefaultEnumValue(),
            aNames.getLength(),
            (rtl_uString **)aNames.getConstArray(),
            const_cast< sal_Int32 * >( aValues.getConstArray() ) );
    }
    return pRet;
}
//==================================================================================================
inline static typelib_TypeDescription * createCTD(
    Reference< container::XHierarchicalNameAccess > const & access,
    const Reference< XIndirectTypeDescription > & xType )
{
    typelib_TypeDescription * pRet = 0;
    if (xType.is())
    {
        typelib_TypeDescription * pRefType = createCTD(
            access, xType->getReferencedType() );
        typelib_typedescription_register( &pRefType );

        OUString aTypeName( xType->getName() );

        typelib_typedescription_new(
            &pRet,
            (typelib_TypeClass)xType->getTypeClass(),
            aTypeName.pData,
            pRefType->pWeakRef,
            0, 0 );

        // cleanup
        if (pRefType)
            typelib_typedescription_release( pRefType );
    }
    return pRet;
}

//==================================================================================================
static typelib_TypeDescription * createCTD(
    Reference< container::XHierarchicalNameAccess > const & access,
    const Reference< XTypeDescription > & xType )
{
    typelib_TypeDescription * pRet = 0;

    if (xType.is())
    {
        switch (xType->getTypeClass())
        {
            // built in types
        case TypeClass_VOID:
        {
            OUString aTypeName( RTL_CONSTASCII_USTRINGPARAM("void") );
            typelib_typedescription_new( &pRet, typelib_TypeClass_VOID, aTypeName.pData, 0, 0, 0 );
            break;
        }
        case TypeClass_CHAR:
        {
            OUString aTypeName( RTL_CONSTASCII_USTRINGPARAM("char") );
            typelib_typedescription_new( &pRet, typelib_TypeClass_CHAR, aTypeName.pData, 0, 0, 0 );
            break;
        }
        case TypeClass_BOOLEAN:
        {
            OUString aTypeName( RTL_CONSTASCII_USTRINGPARAM("boolean") );
            typelib_typedescription_new( &pRet, typelib_TypeClass_BOOLEAN, aTypeName.pData, 0, 0, 0 );
            break;
        }
        case TypeClass_BYTE:
        {
            OUString aTypeName( RTL_CONSTASCII_USTRINGPARAM("byte") );
            typelib_typedescription_new( &pRet, typelib_TypeClass_BYTE, aTypeName.pData, 0, 0, 0 );
            break;
        }
        case TypeClass_SHORT:
        {
            OUString aTypeName( RTL_CONSTASCII_USTRINGPARAM("short") );
            typelib_typedescription_new( &pRet, typelib_TypeClass_SHORT, aTypeName.pData, 0, 0, 0 );
            break;
        }
        case TypeClass_UNSIGNED_SHORT:
        {
            OUString aTypeName( RTL_CONSTASCII_USTRINGPARAM("unsigned short") );
            typelib_typedescription_new( &pRet, typelib_TypeClass_UNSIGNED_SHORT, aTypeName.pData, 0, 0, 0 );
            break;
        }
        case TypeClass_LONG:
        {
            OUString aTypeName( RTL_CONSTASCII_USTRINGPARAM("long") );
            typelib_typedescription_new( &pRet, typelib_TypeClass_LONG, aTypeName.pData, 0, 0, 0 );
            break;
        }
        case TypeClass_UNSIGNED_LONG:
        {
            OUString aTypeName( RTL_CONSTASCII_USTRINGPARAM("unsigned long") );
            typelib_typedescription_new( &pRet, typelib_TypeClass_UNSIGNED_LONG, aTypeName.pData, 0, 0, 0 );
            break;
        }
        case TypeClass_HYPER:
        {
            OUString aTypeName( RTL_CONSTASCII_USTRINGPARAM("hyper") );
            typelib_typedescription_new( &pRet, typelib_TypeClass_HYPER, aTypeName.pData, 0, 0, 0 );
            break;
        }
        case TypeClass_UNSIGNED_HYPER:
        {
            OUString aTypeName( RTL_CONSTASCII_USTRINGPARAM("unsigned hyper") );
            typelib_typedescription_new( &pRet, typelib_TypeClass_UNSIGNED_HYPER, aTypeName.pData, 0, 0, 0 );
            break;
        }
        case TypeClass_FLOAT:
        {
            OUString aTypeName( RTL_CONSTASCII_USTRINGPARAM("float") );
            typelib_typedescription_new( &pRet, typelib_TypeClass_FLOAT, aTypeName.pData, 0, 0, 0 );
            break;
        }
        case TypeClass_DOUBLE:
        {
            OUString aTypeName( RTL_CONSTASCII_USTRINGPARAM("double") );
            typelib_typedescription_new( &pRet, typelib_TypeClass_DOUBLE, aTypeName.pData, 0, 0, 0 );
            break;
        }
        case TypeClass_STRING:
        {
            OUString aTypeName( RTL_CONSTASCII_USTRINGPARAM("string") );
            typelib_typedescription_new( &pRet, typelib_TypeClass_STRING, aTypeName.pData, 0, 0, 0 );
            break;
        }
        case TypeClass_TYPE:
        {
            OUString aTypeName( RTL_CONSTASCII_USTRINGPARAM("type") );
            typelib_typedescription_new( &pRet, typelib_TypeClass_TYPE, aTypeName.pData, 0, 0, 0 );
            break;
        }
        case TypeClass_ANY:
        {
            OUString aTypeName( RTL_CONSTASCII_USTRINGPARAM("any") );
            typelib_typedescription_new( &pRet, typelib_TypeClass_ANY, aTypeName.pData, 0, 0, 0 );
            break;
        }

        case TypeClass_UNION:
            pRet = createCTD( Reference< XUnionTypeDescription >::query( xType ) );
            break;
        case TypeClass_EXCEPTION:
            pRet = createCTD( Reference< XCompoundTypeDescription >::query( xType ) );
            break;
        case TypeClass_STRUCT:
            pRet = createCTD(
                access, Reference< XStructTypeDescription >::query( xType ) );
            break;
        case TypeClass_ENUM:
            pRet = createCTD( Reference< XEnumTypeDescription >::query( xType ) );
            break;
        case TypeClass_TYPEDEF:
        {
            Reference< XIndirectTypeDescription > xTypedef( xType, UNO_QUERY );
            if (xTypedef.is())
                pRet = createCTD( access, xTypedef->getReferencedType() );
            break;
        }
        case TypeClass_SEQUENCE:
            pRet = createCTD(
                access, Reference< XIndirectTypeDescription >::query( xType ) );
            break;
        case TypeClass_INTERFACE:
            pRet = createCTD(
                access,
                Reference< XInterfaceTypeDescription2 >::query( xType ) );
            break;
        case TypeClass_INTERFACE_METHOD:
            pRet = createCTD( Reference< XInterfaceMethodTypeDescription >::query( xType ) );
            break;
        case TypeClass_INTERFACE_ATTRIBUTE:
            pRet = createCTD( Reference< XInterfaceAttributeTypeDescription >::query( xType ) );
            break;
        }
    }

    return pRet;
}


//==================================================================================================
extern "C"
{
static void SAL_CALL typelib_callback(
    void * pContext, typelib_TypeDescription ** ppRet, rtl_uString * pTypeName )
{
    OSL_ENSURE( pContext && ppRet && pTypeName, "### null ptr!" );
    if (ppRet)
    {
        if (*ppRet)
        {
            ::typelib_typedescription_release( *ppRet );
            *ppRet = 0;
        }
        if (pContext && pTypeName)
        {
            Reference< container::XHierarchicalNameAccess > access(
                reinterpret_cast< container::XHierarchicalNameAccess * >(
                    pContext));
            try
            {
                OUString const & rTypeName = OUString::unacquired( &pTypeName );
                Reference< XTypeDescription > xTD;
                if (access->getByHierarchicalName(rTypeName ) >>= xTD)
                {
                    *ppRet = createCTD( access, xTD );
                }
            }
            catch (container::NoSuchElementException & exc)
            {
                OSL_ENSURE(
                    0, OUStringToOString(
                        OUString( RTL_CONSTASCII_USTRINGPARAM(
                                      "typelibrary type not available: ") ) +
                        exc.Message, RTL_TEXTENCODING_UTF8 ).getStr() );
            }
            catch (Exception & exc)
            {
                OSL_ENSURE(
                    0, OUStringToOString(
                        exc.Message, RTL_TEXTENCODING_UTF8 ).getStr() );
            }
        }
    }
}
}

//==================================================================================================
class EventListenerImpl
    : public WeakImplHelper1< lang::XEventListener >
{
    Reference< container::XHierarchicalNameAccess > m_xTDMgr;

public:
    inline EventListenerImpl(
        Reference< container::XHierarchicalNameAccess > const & xTDMgr )
        SAL_THROW( () )
        : m_xTDMgr( xTDMgr )
        {}

    // XEventListener
    virtual void SAL_CALL disposing( lang::EventObject const & rEvt )
        throw (RuntimeException);
};
//__________________________________________________________________________________________________
void EventListenerImpl::disposing( lang::EventObject const & rEvt )
    throw (RuntimeException)
{
    OSL_ASSERT( rEvt.Source == m_xTDMgr );
    // deregister of c typelib callback
    ::typelib_typedescription_revokeCallback( m_xTDMgr.get(), typelib_callback );
}

//==================================================================================================
sal_Bool SAL_CALL installTypeDescriptionManager(
    Reference< container::XHierarchicalNameAccess > const & xTDMgr )
    SAL_THROW( () )
{
    Reference< lang::XComponent > xComp( xTDMgr, UNO_QUERY );
    if (xComp.is())
    {
        xComp->addEventListener( new EventListenerImpl( xTDMgr ) );
        // register c typelib callback
        ::typelib_typedescription_registerCallback( xTDMgr.get(), typelib_callback );
        return sal_True;
    }
    return sal_False;
}

} // end namespace cppu

