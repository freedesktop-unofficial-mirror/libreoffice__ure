/*************************************************************************
 *
 *  $RCSfile: typeblop.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2003-12-01 17:56:47 $
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

#ifndef _RTL_ALLOC_H_
#include <rtl/alloc.h>
#endif
#ifndef __REGISTRY_REFLWRIT_HXX__
#include <registry/reflwrit.hxx>
#endif

#ifndef _CPPUHELPER_SERVICEFACTORY_HXX_
#include <cppuhelper/servicefactory.hxx>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XHIERARCHICALNAMEACCESS_HPP_
#include <com/sun/star/container/XHierarchicalNameAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_REFLECTION_XINTERFACETYPEDESCRIPTION_HPP_
#include <com/sun/star/reflection/XInterfaceTypeDescription.hpp>
#endif
#ifndef _COM_SUN_STAR_REFLECTION_XCONSTANTSTYPEDESCRIPTION_HPP_
#include <com/sun/star/reflection/XConstantsTypeDescription.hpp>
#endif
#ifndef _COM_SUN_STAR_REFLECTION_XCONSTANTTYPEDESCRIPTION_HPP_
#include <com/sun/star/reflection/XConstantTypeDescription.hpp>
#endif
#ifndef _COM_SUN_STAR_REFLECTION_XMODULETYPEDESCRIPTION_HPP_
#include <com/sun/star/reflection/XModuleTypeDescription.hpp>
#endif
#ifndef _COM_SUN_STAR_REFLECTION_XINTERFACEMETHODTYPEDESCRIPTION_HPP_
#include <com/sun/star/reflection/XInterfaceMethodTypeDescription.hpp>
#endif
#ifndef _COM_SUN_STAR_REFLECTION_XINTERFACEATTRIBUTETYPEDESCRIPTION_HPP_
#include <com/sun/star/reflection/XInterfaceAttributeTypeDescription.hpp>
#endif
#ifndef _COM_SUN_STAR_REFLECTION_XMETHODPARAMETER_HPP_
#include <com/sun/star/reflection/XMethodParameter.hpp>
#endif
#ifndef _COM_SUN_STAR_REFLECTION_XCOMPOUNDTYPEDESCRIPTION_HPP_
#include <com/sun/star/reflection/XCompoundTypeDescription.hpp>
#endif
#ifndef _COM_SUN_STAR_REFLECTION_XINDIRECTTYPEDESCRIPTION_HPP_
#include <com/sun/star/reflection/XIndirectTypeDescription.hpp>
#endif
#ifndef _COM_SUN_STAR_REFLECTION_XENUMTYPEDESCRIPTION_HPP_
#include <com/sun/star/reflection/XEnumTypeDescription.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_XCOMPONENTCONTEXT_HPP_
#include <com/sun/star/uno/XComponentContext.hpp>
#endif

#ifndef _CODEMAKER_GLOBAL_HXX_
#include <codemaker/global.hxx>
#endif

using namespace com::sun::star;
using namespace com::sun::star::uno;
using namespace com::sun::star::reflection;
using namespace com::sun::star::lang;
using namespace com::sun::star::container;
using namespace cppu;
//using namespace osl;
using namespace rtl;

static Reference< XHierarchicalNameAccess > xNameAccess;

void writeConstantData( RegistryTypeWriter& rWriter, sal_uInt16 fieldIndex,
                      const Reference< XConstantTypeDescription >& xConstant)
    
{					  
    RTConstValue constValue;						  
    OUString uConstTypeName;
    OUString uConstName = xConstant->getName();
    Any aConstantAny = xConstant->getConstantValue();
    
    switch ( aConstantAny.getValueTypeClass() )
    {
        case TypeClass_BOOLEAN:
            {
                uConstTypeName = OUString( RTL_CONSTASCII_USTRINGPARAM("boolean") );
                constValue.m_type = RT_TYPE_BOOL; 
                aConstantAny >>= constValue.m_value.aBool;
            }
            break;
        case TypeClass_BYTE:
            {
                uConstTypeName = OUString( RTL_CONSTASCII_USTRINGPARAM("byte") );
                constValue.m_type = RT_TYPE_BYTE; 
                aConstantAny >>= constValue.m_value.aByte;
            }
            break;
        case TypeClass_SHORT:
            {
                uConstTypeName = OUString( RTL_CONSTASCII_USTRINGPARAM("short") );
                constValue.m_type = RT_TYPE_INT16; 
                aConstantAny >>= constValue.m_value.aShort;
            }
            break;
        case TypeClass_UNSIGNED_SHORT:
            {
                uConstTypeName = OUString( RTL_CONSTASCII_USTRINGPARAM("unsigned short") );
                constValue.m_type = RT_TYPE_UINT16; 
                aConstantAny >>= constValue.m_value.aUShort;
            }
            break;
        case TypeClass_LONG:
            {
                uConstTypeName = OUString( RTL_CONSTASCII_USTRINGPARAM("long") );
                constValue.m_type = RT_TYPE_INT32; 
                aConstantAny >>= constValue.m_value.aLong;
            }
            break;
        case TypeClass_UNSIGNED_LONG:
            {
                uConstTypeName = OUString( RTL_CONSTASCII_USTRINGPARAM("unsigned long") );
                constValue.m_type = RT_TYPE_UINT32; 
                aConstantAny >>= constValue.m_value.aULong;
            }
            break;
        case TypeClass_FLOAT:
            {
                uConstTypeName = OUString( RTL_CONSTASCII_USTRINGPARAM("float") );
                constValue.m_type = RT_TYPE_FLOAT; 
                aConstantAny >>= constValue.m_value.aFloat;
            }
            break;
        case TypeClass_DOUBLE:
            {
                uConstTypeName = OUString( RTL_CONSTASCII_USTRINGPARAM("double") );
                constValue.m_type = RT_TYPE_DOUBLE; 
                aConstantAny >>= constValue.m_value.aDouble;
            }
            break;
        case TypeClass_STRING:
            {
                uConstTypeName = OUString( RTL_CONSTASCII_USTRINGPARAM("string") );
                constValue.m_type = RT_TYPE_STRING; 
                constValue.m_value.aString = ((OUString*)aConstantAny.getValue())->getStr();
            }
            break;
    }

    rWriter.setFieldData(fieldIndex, uConstName, uConstTypeName, OUString(), 
                        OUString(), RT_ACCESS_CONST, constValue);
}	

sal_uInt32 getInheritedMemberCount( Reference< XTypeDescription >& xType )
{
    sal_uInt32 memberCount = 0;
    if ( xType->getTypeClass() == TypeClass_INTERFACE )
    {
        Reference< XInterfaceTypeDescription > xIFace(xType, UNO_QUERY);

        if ( !xIFace.is() )
            return memberCount;

        Reference< XTypeDescription > xSuperType = xIFace->getBaseType();

        if ( xSuperType.is() )
            memberCount = getInheritedMemberCount( xSuperType );

        memberCount += xIFace->getMembers().getLength();
    }
//	} else
//	if ( xType->getTypeClass() == TypeClass_Struct || xType->getTypeClass() == TypeClass_Exception )
//	{
//		Reference< XCompoundTypeDescription > xComp(xType, UNO_QUERY);
//
//		if ( xComp.is() )
//			return membercount;
//
//		Reference< XTypeDescription > xSuperType = xComp->getBaseType();
//
//		if ( xSuperType.is() )
//			memberCount = getInheritedMemberCount( xSuperType );
//
//		memberCount += xComp->getMemberNames().getLength();
//	}

    return memberCount;
}

void writeMethodData( RegistryTypeWriter& rWriter, sal_uInt32 calculatedMemberOffset,
                      const Reference< XInterfaceMemberTypeDescription >& xMember,
                      const Reference< XInterfaceMethodTypeDescription >& xMethod )
{
    RTMethodMode methodMode = RT_MODE_TWOWAY;
    if ( xMethod->isOneway() )
    {
        methodMode = RT_MODE_ONEWAY;	
    }
    
    Sequence< Reference< XMethodParameter > > parameters( xMethod->getParameters() );
    Sequence< Reference< XTypeDescription > > exceptions( xMethod->getExceptions() );
    sal_uInt16 methodIndex = (sal_uInt16)(xMember->getPosition() - calculatedMemberOffset);
    sal_uInt16 paramCount = (sal_uInt16)parameters.getLength();
    sal_uInt16 exceptionCount = (sal_uInt16)exceptions.getLength();
    
    rWriter.setMethodData(methodIndex, xMember->getMemberName(),
                          xMethod->getReturnType()->getName().replace('.', '/'), 
                          methodMode, paramCount, exceptionCount, OUString());

    RTParamMode paramMode = RT_PARAM_IN;
    sal_uInt16 i;

    for ( i=0; i < paramCount; i++)
    {
        Reference< XMethodParameter > xParam = parameters[i];
        if ( xParam->isIn() && xParam->isOut())
        {
            paramMode = RT_PARAM_INOUT;
        } else
        if ( xParam->isIn() )
        {
            paramMode = RT_PARAM_IN;				
        } else
        if ( xParam->isOut() )
        {
            paramMode = RT_PARAM_OUT;
        }

        rWriter.setParamData(methodIndex, (sal_uInt16)xParam->getPosition(), xParam->getType()->getName().replace('.', '/'), 
                             xParam->getName(), paramMode); 
    }
        
    for (i=0; i < exceptionCount; i++)
    {
        rWriter.setExcData(methodIndex, i, exceptions[i]->getName().replace('.', '/')); 
    }
}	

extern "C" 
{

sal_Bool SAL_CALL initTypeMapper( const sal_Char* pRegName )
{
    try
    {
        if (!pRegName)
            return sal_False;

        Reference< XMultiServiceFactory > xSMgr( createRegistryServiceFactory( convertToFileUrl(pRegName) ) );

        if ( !xSMgr.is() )
            return sal_False;

        Reference< XHierarchicalNameAccess > xNAccess;
        
        Reference< beans::XPropertySet > xProps( xSMgr, UNO_QUERY );
        if (xProps.is())
        {
            try
            {
                Reference< XComponentContext > xContext;
                if (xProps->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("DefaultContext") ) ) >>= xContext)
                {
                    xContext->getValueByName(
                        OUString( RTL_CONSTASCII_USTRINGPARAM("/singletons/com.sun.star.reflection.theTypeDescriptionManager") ) ) >>= xNAccess;
                }
            }
            catch (beans::UnknownPropertyException &)
            {
            }
        }
        
        if ( !xNAccess.is() )
            return sal_False;
        
        xNameAccess = xNAccess;
    }
    catch( Exception& )
    {
        return sal_False;
    }

    return sal_True;
}	

sal_uInt32 SAL_CALL getTypeBlop(const sal_Char* pTypeName, sal_uInt8** pBlop)
{
    sal_uInt32 length = 0;

    if ( !pTypeName )
        return length;

    OUString uTypeName( OUString::createFromAscii(pTypeName).replace('/', '.') );
    try
    {
        Any aTypeAny( xNameAccess->getByHierarchicalName( uTypeName ) );

        if ( !aTypeAny.hasValue() )
            return length;

        RegistryTypeWriterLoader rtwLoader;
        if ( !rtwLoader.isLoaded() )
            return length;

        Reference< XTypeDescription > xType;
        aTypeAny >>= xType;	

        if ( !xType.is() )
            return length;

        switch (xType->getTypeClass())
        {
            case TypeClass_CONSTANTS:
                {
                    Reference< XConstantsTypeDescription > xCFace(xType, UNO_QUERY);
                    
                    if ( !xCFace.is() )
                        return length;
                    
                  Sequence< Reference< XConstantTypeDescription > > constTypes( xCFace->getConstants());
                    sal_uInt16 constCount = (sal_uInt16)constTypes.getLength();
                    
                    RegistryTypeWriter writer(rtwLoader, RT_TYPE_MODULE, uTypeName.replace('.', '/'), 
                                              OUString(), constCount, 0, 0);
                    
                  for (sal_uInt16 i=0; i < constCount; i++)
                      writeConstantData(writer, i, constTypes[i]); 
                    
                    length = writer.getBlopSize();
                    *pBlop = (sal_uInt8*)rtl_allocateMemory( length );
                    rtl_copyMemory(*pBlop, writer.getBlop(), length);
                }
                break;
            case TypeClass_MODULE:
                {
                    Reference< XModuleTypeDescription > xMFace(xType, UNO_QUERY);
                    
                    if ( !xMFace.is() )
                        return length;

                  Sequence< Reference< XTypeDescription > > memberTypes( xMFace->getMembers());
                  
                    sal_uInt16 memberCount = (sal_uInt16)memberTypes.getLength();
                    sal_uInt16 constCount = 0;
                    sal_Int16 i;

                    for ( i=0; i < memberCount; i++)
                    {
                        if ( TypeClass_CONSTANT == memberTypes[i]->getTypeClass() )
                            constCount++;
                    }

                    RegistryTypeWriter writer(rtwLoader, RT_TYPE_MODULE, uTypeName.replace('.', '/'), 
                                              OUString(), constCount, 0, 0);
                    
                    if ( 0 < constCount )
                  {
                      Reference< XConstantTypeDescription > xConst;
                      sal_uInt16 fieldIndex = 0;  
                      for (i=0; i < memberCount; i++)
                      {
                          if ( TypeClass_CONSTANT == memberTypes[i]->getTypeClass() )
                          {
                              xConst = Reference< XConstantTypeDescription >(memberTypes[i], UNO_QUERY);
                              
                              writeConstantData(writer, ++fieldIndex, xConst); 
                          }
                      }
                  }
                    
                    length = writer.getBlopSize();
                    *pBlop = (sal_uInt8*)rtl_allocateMemory( length );
                    rtl_copyMemory(*pBlop, writer.getBlop(), length);                    
                }
                break;
            case TypeClass_INTERFACE:
                {
                    Reference< XInterfaceTypeDescription > xIFace(xType, UNO_QUERY);
                    
                    if ( !xIFace.is() )
                        return length;

                    Reference< XInterfaceAttributeTypeDescription > xAttr;
                    Reference< XInterfaceMethodTypeDescription > xMethod;
                    Sequence< Reference< XInterfaceMemberTypeDescription > > memberTypes( xIFace->getMembers());
                    sal_uInt16 memberCount = (sal_uInt16)memberTypes.getLength();
                    sal_uInt16 attrCount = 0;
                    sal_uInt16 inheritedMemberCount = 0;
                    sal_Int32 i;

                    for ( i=0; i < memberCount; i++)
                    {
                        xAttr = Reference< XInterfaceAttributeTypeDescription >(memberTypes[i], UNO_QUERY);
                        if ( xAttr.is() )
                        {
                            attrCount++;
                        }
                    }
                    
                    OUString uSuperType;
                    Reference< XTypeDescription > xSuperType = xIFace->getBaseType();
                    if ( xSuperType.is() )
                    {
                        uSuperType = xSuperType->getName().replace('.','/');
                        inheritedMemberCount = (sal_uInt16)getInheritedMemberCount( xSuperType );
                    }

                    RegistryTypeWriter writer(rtwLoader, RT_TYPE_INTERFACE, uTypeName.replace('.', '/'), 
                                              uSuperType, attrCount, memberCount-attrCount, 0);
                    
                    Uik	  uik = xIFace->getUik();
                    RTUik rtUik = { uik.m_Data1, uik.m_Data2, uik.m_Data3, uik.m_Data4, uik.m_Data5 };
                    writer.setUik( rtUik );
                    
                    RTFieldAccess attrAccess = RT_ACCESS_READWRITE;
                    // reset attrCount, used for method index calculation
                    attrCount = 0;                    
                    
                    for (i=0; i < memberCount; i++)
                    {
                        xAttr = Reference< XInterfaceAttributeTypeDescription >(memberTypes[i], UNO_QUERY);
                        if ( xAttr.is() )
                        {
                            ++attrCount;                            
                            if (xAttr->isReadOnly())
                            {
                                attrAccess = RT_ACCESS_READONLY;	
                            } else
                            {
                                attrAccess = RT_ACCESS_READWRITE;	
                            }							
                            writer.setFieldData(memberTypes[i]->getPosition() - inheritedMemberCount,
                                                memberTypes[i]->getMemberName(),
                                                xAttr->getType()->getName().replace('.', '/'), 
                                                OUString(), OUString(), attrAccess);
                            continue;
                        }

                        xMethod = Reference< XInterfaceMethodTypeDescription >(memberTypes[i], UNO_QUERY);
                        if ( xMethod.is() )
                        {
                            writeMethodData( writer, attrCount+inheritedMemberCount, memberTypes[i], xMethod );
                        }
                    }
                                              
                    length = writer.getBlopSize();
                    *pBlop = (sal_uInt8*)rtl_allocateMemory( length );
                    rtl_copyMemory(*pBlop, writer.getBlop(), length);
                }
                break;
            case TypeClass_STRUCT:
            case TypeClass_EXCEPTION:
                {
                    RTTypeClass rtTypeClass = RT_TYPE_STRUCT;
                    if (xType->getTypeClass() == TypeClass_EXCEPTION)
                    {
                        rtTypeClass = RT_TYPE_EXCEPTION;	
                    }
#ifndef _COM_SUN_STAR_REFLECTION_XCONSTANTSTYPEDESCRIPTION_HPP_
#include <com/sun/star/reflection/XConstantsTypeDescription.hpp>
#endif

                    Reference< XCompoundTypeDescription > xComp(xType, UNO_QUERY);
                    
                    if ( !xComp.is() )
                        return length;

                    Sequence< OUString > memberNames( xComp->getMemberNames());
                    Sequence< Reference< XTypeDescription > > memberTypes( xComp->getMemberTypes());
                    sal_uInt16 memberCount = (sal_uInt16)memberNames.getLength();
                    
                    OUString uSuperType;
                    Reference< XTypeDescription > xSuperType = xComp->getBaseType();
                    if ( xSuperType.is() )
                    {
                        uSuperType = xSuperType->getName().replace('.','/');
                    }

                    RegistryTypeWriter writer(rtwLoader, rtTypeClass, uTypeName.replace('.', '/'),
                                              uSuperType, memberCount, 0, 0);
                    
                    for (sal_Int16 i=0; i < memberCount; i++)
                    {
                        writer.setFieldData(i , memberNames[i], memberTypes[i]->getName().replace('.', '/'), 
                                            OUString(), OUString(), RT_ACCESS_READWRITE);
                    }
                                              
                    length = writer.getBlopSize();
                    *pBlop = (sal_uInt8*)rtl_allocateMemory( length );
                    rtl_copyMemory(*pBlop, writer.getBlop(), length);
                }
                break;
            case TypeClass_ENUM:
                {
                    Reference< XEnumTypeDescription > xEnum(xType, UNO_QUERY);
                    
                    if ( !xEnum.is() )
                        return length;

                    Sequence< OUString > enumNames( xEnum->getEnumNames());
                    Sequence< sal_Int32 > enumValues( xEnum->getEnumValues());
                    sal_uInt16 enumCount = (sal_uInt16)enumNames.getLength();
                    
                    RegistryTypeWriter writer(rtwLoader, RT_TYPE_ENUM, uTypeName.replace('.', '/'),
                                              OUString(), enumCount, 0, 0);
                    
                    RTConstValue constValue;						  
                    for (sal_Int16 i=0; i < enumCount; i++)
                    {
                        constValue.m_type = RT_TYPE_INT32; 
                        constValue.m_value.aLong = enumValues[i];
                        
                        writer.setFieldData(i, enumNames[i], OUString(), OUString(), OUString(),
                                            RT_ACCESS_CONST, constValue);
                    }
                                              
                    length = writer.getBlopSize();
                    *pBlop = (sal_uInt8*)rtl_allocateMemory( length );
                    rtl_copyMemory(*pBlop, writer.getBlop(), length);
                }
                break;
            case TypeClass_TYPEDEF:
                {
                    Reference< XIndirectTypeDescription > xTD(xType, UNO_QUERY);
                    
                    if ( !xTD.is() )
                        return length;

                    RegistryTypeWriter writer(rtwLoader, RT_TYPE_TYPEDEF, uTypeName.replace('.', '/'),
                                              xTD->getReferencedType()->getName().replace('.', '/'),
                                              0, 0, 0);
                    length = writer.getBlopSize();
                    *pBlop = (sal_uInt8*)rtl_allocateMemory( length );
                    rtl_copyMemory(*pBlop, writer.getBlop(), length);
                }
                break;
        }

    }
    catch( Exception& )
    {
    }

    return length;	
}	

} // extern "C"


    
