/*************************************************************************
 *
 *  $RCSfile: testregcpp.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: jl $ $Date: 2001-03-12 16:59:01 $
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

#include <iostream.h>
#include <stdio.h>
#include <string.h>

#include "registry/registry.hxx"
#include "registry/reflread.hxx"
#include "registry/reflwrit.hxx"



#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

#ifndef _RTL_ALLOC_H_
#include <rtl/alloc.h>
#endif

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

using namespace rtl;

void test_coreReflection()
{
    RegistryLoader* pLoader = new RegistryLoader();

    if (!pLoader->isLoaded())
    {
        delete pLoader;
        return;
    }

    RegistryTypeWriterLoader* pWriterLoader = new RegistryTypeWriterLoader();

    if (!pWriterLoader->isLoaded())
    {
        delete pWriterLoader;
        return;
    }

    RegistryTypeReaderLoader* pReaderLoader = new RegistryTypeReaderLoader();

    if (!pReaderLoader->isLoaded())
    {
        delete pReaderLoader;
        return;
    }

    Registry *myRegistry = new Registry(*pLoader);
    delete pLoader;

    RegistryKey rootKey, key1, key2, key3, key4 ,key5, key6, key7, key8;

    OSL_ENSURE(!myRegistry->create(OUString::createFromAscii("ucrtest.rdb")), "testCoreReflection error 1");
    OSL_ENSURE(!myRegistry->openRootKey(rootKey), "testCoreReflection error 2");

    OSL_ENSURE(!rootKey.createKey(OUString::createFromAscii("UCR"), key1), "testCoreReflection error 3");
    OSL_ENSURE(!key1.createKey(OUString::createFromAscii("ModuleA"), key2), "testCoreReflection error 4");
    OSL_ENSURE(!key2.createKey(OUString::createFromAscii("StructA"), key3), "testCoreReflection error 5");
    OSL_ENSURE(!key2.createKey(OUString::createFromAscii("EnumA"), key4), "testCoreReflection error 6");
    OSL_ENSURE(!key2.createKey(OUString::createFromAscii("XInterfaceA"), key5), "testCoreReflection error 7");
    OSL_ENSURE(!key2.createKey(OUString::createFromAscii("ExceptionA"), key6), "testCoreReflection error 8");
    OSL_ENSURE(!key2.createKey(OUString::createFromAscii("ServiceA"), key7), "testCoreReflection error 8a");
    OSL_ENSURE(!key2.createKey(OUString::createFromAscii("ConstantsA"), key8), "testCoreReflection error 8b");

    {
        RegistryTypeWriter writer(*pWriterLoader,
                                  RT_TYPE_MODULE,
                                  OUString::createFromAscii("ModuleA"),
                                  OUString(), 9, 0, 0);

        RTConstValue aConst;

        writer.setDoku(OUString::createFromAscii("Hallo ich bin ein Modul"));
        writer.setFileName(OUString::createFromAscii("DummyFile"));

        aConst.m_type = RT_TYPE_BOOL;
        aConst.m_value.aBool = sal_True;
        writer.setFieldData(0, OUString::createFromAscii("aConstBool"), 
                            OUString::createFromAscii("boolean"), 
                            OUString::createFromAscii("ich bin ein boolean"), 
                            OUString::createFromAscii("DummyFile"), RT_ACCESS_CONST, aConst);
        aConst.m_type = RT_TYPE_BYTE;
        aConst.m_value.aByte = 127;
        writer.setFieldData(1, OUString::createFromAscii("aConstByte"), 
                            OUString::createFromAscii("byte"), 
                            OUString::createFromAscii("ich bin ein byte"), 
                            OUString::createFromAscii("DummyFile"),  RT_ACCESS_CONST, aConst);
        aConst.m_type = RT_TYPE_INT16;
        aConst.m_value.aShort = -10;
        writer.setFieldData(2, OUString::createFromAscii("aConstShort"), 
                            OUString::createFromAscii("short"), 
                            OUString::createFromAscii("ich bin ein short"), 
                            OUString::createFromAscii("DummyFile"),  RT_ACCESS_CONST, aConst);
        aConst.m_type = RT_TYPE_UINT16;
        aConst.m_value.aUShort = 10;
        writer.setFieldData(3, OUString::createFromAscii("aConstUShort"), 
                            OUString::createFromAscii("unsigned short"), 
                            OUString::createFromAscii("ich bin ein unsigned short"), 
                            OUString::createFromAscii("DummyFile"), RT_ACCESS_CONST, aConst);
        aConst.m_type = RT_TYPE_INT32;
        aConst.m_value.aLong = -100000;
        writer.setFieldData(4, OUString::createFromAscii("aConstLong"), 
                            OUString::createFromAscii("long"), 
                            OUString::createFromAscii("ich bin ein long"), 
                            OUString::createFromAscii("DummyFile"), RT_ACCESS_CONST, aConst);
        aConst.m_type = RT_TYPE_UINT32;
        aConst.m_value.aULong = 100000;
        writer.setFieldData(5, OUString::createFromAscii("aConstULong"), 
                            OUString::createFromAscii("unsigned long"), 
                            OUString::createFromAscii("ich bin ein unsigned long"), 
                            OUString::createFromAscii("DummyFile"), RT_ACCESS_CONST, aConst);
        aConst.m_type = RT_TYPE_FLOAT;
        aConst.m_value.aFloat = -2e-10f;
        writer.setFieldData(6, OUString::createFromAscii("aConstFloat"), 
                            OUString::createFromAscii("float"), 
                            OUString::createFromAscii("ich bin ein float"), 
                            OUString::createFromAscii("DummyFile"), RT_ACCESS_CONST, aConst);
        aConst.m_type = RT_TYPE_DOUBLE;
        aConst.m_value.aDouble = -2e-100; writer.setFieldData(7, OUString::createFromAscii("aConstDouble"), 
                                                              OUString::createFromAscii("double"), 
                                                              OUString::createFromAscii("ich bin ein double"), 
                                                              OUString::createFromAscii("DummyFile"), RT_ACCESS_CONST, aConst);
        aConst.m_type = RT_TYPE_STRING;
        OUString tmpStr(OUString::createFromAscii( "dies ist ein unicode string" ));
        aConst.m_value.aString = tmpStr.getStr();

        writer.setFieldData(8, OUString::createFromAscii("aConstString"), 
                            OUString::createFromAscii("string"), 
                            OUString::createFromAscii("ich bin ein string"), 
                            OUString::createFromAscii("DummyFile"), RT_ACCESS_CONST, aConst);

        const sal_uInt8* pBlop = writer.getBlop();
        sal_uInt32      aBlopSize = writer.getBlopSize();

        OSL_ENSURE(!key2.setValue(OUString(), RG_VALUETYPE_BINARY, (void*)pBlop, aBlopSize), "testCoreReflection error 9");
    
        sal_uInt8* readBlop = (sal_uInt8*)rtl_allocateMemory(aBlopSize);
        OSL_ENSURE(!key2.getValue(OUString(), (void*)readBlop) , "testCoreReflection error 9a");
    
        RegistryTypeReader reader(*pReaderLoader, readBlop, aBlopSize, sal_True);

        if (reader.isValid())
        {
            OSL_ENSURE(reader.getTypeName().equals(OUString::createFromAscii("ModuleA")), "testCoreReflection error 9a2");

            RTConstValue aReadConst = reader.getFieldConstValue(8);
            OString aConstStr = OUStringToOString(aConst.m_value.aString, RTL_TEXTENCODING_ASCII_US);
            OSL_ENSURE(aConstStr.equals("dies ist ein unicode string"), "testCoreReflection error 9b");
        }	
    
    }

    {
        RegistryTypeWriter writer(*pWriterLoader,
                                  RT_TYPE_STRUCT,
                                  OUString::createFromAscii("ModuleA/StructA"),
                                  OUString(), 3, 0, 0);

        writer.setDoku(OUString::createFromAscii("Hallo ich bin eine Struktur"));
        writer.setFileName(OUString::createFromAscii("DummyFile"));

        writer.setFieldData(0, OUString::createFromAscii("asal_uInt32"), 
                            OUString::createFromAscii("unsigned long"),
                            OUString(), OUString(), RT_ACCESS_READWRITE);
        writer.setFieldData(1, OUString::createFromAscii("aXInterface"), 
                            OUString::createFromAscii("stardiv/uno/XInterface"), 
                            OUString(), OUString(), RT_ACCESS_READWRITE);
        writer.setFieldData(2, OUString::createFromAscii("aSequence"), 
                            OUString::createFromAscii("[]ModuleA/EnumA"), 
                            OUString(), OUString(), RT_ACCESS_READWRITE);

        const sal_uInt8* pBlop = writer.getBlop();
        sal_uInt32      aBlopSize = writer.getBlopSize();

        OSL_ENSURE(!key3.setValue(OUString(), RG_VALUETYPE_BINARY, (void*)pBlop, aBlopSize), "testCoreReflection error 9a");    
    }

    {
        RegistryTypeWriter writer(*pWriterLoader,
                                  RT_TYPE_ENUM,
                                  OUString::createFromAscii("ModuleA/EnumA"),
                                  OUString(), 2, 0, 0);

        RTConstValue aConst;

        aConst.m_type = RT_TYPE_UINT32;
        aConst.m_value.aULong = 10;

        writer.setDoku(OUString::createFromAscii("Hallo ich bin ein Enum"));
        writer.setFileName(OUString::createFromAscii("DummyFile"));

        writer.setFieldData(0, OUString::createFromAscii("ENUM_VAL_1"), 
                            OUString(), OUString::createFromAscii("ich bin ein enum value"), 
                            OUString(), RT_ACCESS_CONST, aConst);

        aConst.m_value.aULong = 10;
        writer.setFieldData(1, OUString::createFromAscii("ENUM_VAL_2"), 
                            OUString(), OUString(), OUString(), RT_ACCESS_CONST, aConst);

        const sal_uInt8* pBlop = writer.getBlop();
        sal_uInt32      aBlopSize = writer.getBlopSize();

        OSL_ENSURE(!key4.setValue(OUString(), RG_VALUETYPE_BINARY, (void*)pBlop, aBlopSize), "testCoreReflection error 9b");
    }

    {
        RegistryTypeWriter writer(*pWriterLoader,
                                  RT_TYPE_INTERFACE,
                                  OUString::createFromAscii("ModuleA/XInterfaceA"),
                                  OUString::createFromAscii("stardiv/uno/XInterface"),
                                  4, 1, 0);
        RTConstValue aConst;

        RTUik aUik = {1,2,3,4,5};

        writer.setUik(aUik);
        writer.setDoku(OUString::createFromAscii("Hallo ich bin ein Interface"));
        writer.setFileName(OUString::createFromAscii("DummyFile"));

        writer.setFieldData(0, OUString::createFromAscii("aString"), 
                            OUString::createFromAscii("string"), OUString(), OUString(), RT_ACCESS_READWRITE);
        writer.setFieldData(1, OUString::createFromAscii("aStruct"), 
                            OUString::createFromAscii("ModuleA/StructA"), 
                            OUString(), OUString(), RT_ACCESS_READONLY);
        writer.setFieldData(2, OUString::createFromAscii("aEnum"), 
                            OUString::createFromAscii("ModuleA/EnumA"), OUString(), OUString(), RT_ACCESS_BOUND);
        aConst.m_type = RT_TYPE_UINT16;
        aConst.m_value.aUShort = 12;
        writer.setFieldData(3, OUString::createFromAscii("aConstUShort"), 
                            OUString::createFromAscii("unsigned short"), OUString(), 
                            OUString::createFromAscii("DummyFile"), RT_ACCESS_CONST, aConst);

        writer.setMethodData(0, OUString::createFromAscii("methodA"), 
                             OUString::createFromAscii("double"), RT_MODE_TWOWAY, 2, 1, 
                             OUString::createFromAscii("Hallo ich bin die methodA"));
        writer.setParamData(0, 0, OUString::createFromAscii("ModuleA/StructA"), 
                            OUString::createFromAscii("aStruct"), RT_PARAM_IN);
        writer.setParamData(0, 1, OUString::createFromAscii("unsigned short"), 
                            OUString::createFromAscii("aShort"), RT_PARAM_INOUT);
        writer.setExcData(0, 0, OUString::createFromAscii("ModuleA/ExceptionA"));

        const sal_uInt8* pBlop = writer.getBlop();
        sal_uInt32      aBlopSize = writer.getBlopSize();

        OSL_ENSURE(!key5.setValue(OUString(), RG_VALUETYPE_BINARY, (void*)pBlop, aBlopSize), "testCoreReflection error 9c");

        sal_uInt8* readBlop = (sal_uInt8*)rtl_allocateMemory(aBlopSize);
        OSL_ENSURE(!key5.getValue(OUString(), (void*)readBlop) , "testCoreReflection error 9c1");
    
        RegistryTypeReader reader(*pReaderLoader, readBlop, aBlopSize, sal_True);

        if (reader.isValid())
        {
            OSL_ENSURE(reader.getTypeName().equals(OUString::createFromAscii("ModuleA/XInterfaceA")), "testCoreReflection error 9c2");

            RTUik retUik;
            reader.getUik(retUik);
            OSL_ENSURE(retUik.m_Data1 = 1, "testCoreReflection error 9c3");
            OSL_ENSURE(retUik.m_Data2 = 2, "testCoreReflection error 9c4");
            OSL_ENSURE(retUik.m_Data3 = 3, "testCoreReflection error 9c5");
            OSL_ENSURE(retUik.m_Data4 = 4, "testCoreReflection error 9c6");
            OSL_ENSURE(retUik.m_Data5 = 5, "testCoreReflection error 9c7");
        }	

    }

    {
        RegistryTypeWriter writer(*pWriterLoader,
                                  RT_TYPE_EXCEPTION,
                                  OUString::createFromAscii("ModuleA/ExceptionA"),
                                  OUString(), 1, 0, 0);

        writer.setDoku(OUString::createFromAscii("Hallo ich bin eine Exception"));

        writer.setFieldData(0, OUString::createFromAscii("aSource"), 
                            OUString::createFromAscii("stardiv/uno/XInterface"), 
                            OUString::createFromAscii("ich bin ein interface member"), 
                            OUString(), RT_ACCESS_READWRITE);

        const sal_uInt8* pBlop = writer.getBlop();
        sal_uInt32      aBlopSize = writer.getBlopSize();

        OSL_ENSURE(!key6.setValue(OUString(), RG_VALUETYPE_BINARY, (void*)pBlop, aBlopSize), "testCoreReflection error 9d");
    }

    {
        RegistryTypeWriter writer(*pWriterLoader,
                                  RT_TYPE_SERVICE,
                                  OUString::createFromAscii("ModuleA/ServiceA"),
                                  OUString(), 1, 0, 4);

        writer.setDoku(OUString::createFromAscii("Hallo ich bin ein Service"));
        writer.setFileName(OUString::createFromAscii("DummyFile"));

        writer.setFieldData(0, OUString::createFromAscii("aProperty"), 
                            OUString::createFromAscii("stardiv/uno/XInterface"), 
                            OUString::createFromAscii("ich bin eine property"), 
                            OUString(), RT_ACCESS_READWRITE);

        writer.setReferenceData(0, OUString::createFromAscii("ModuleA/XInterfaceA"), RT_REF_SUPPORTS,
                                OUString::createFromAscii("Hallo ich eine Reference auf ein supported interface"),
                                RT_ACCESS_OPTIONAL);
        writer.setReferenceData(1, OUString::createFromAscii("ModuleA/XInterfaceA"), RT_REF_OBSERVES,
                                OUString::createFromAscii("Hallo ich eine Reference auf ein observed interface"));
        writer.setReferenceData(2, OUString::createFromAscii("ModuleA/ServiceB"), RT_REF_EXPORTS,
                                OUString::createFromAscii("Hallo ich eine Reference auf einen exported service"));
        writer.setReferenceData(3, OUString::createFromAscii("ModuleA/ServiceB"), RT_REF_NEEDS,
                                OUString::createFromAscii("Hallo ich eine Reference auf einen needed service"));

        const sal_uInt8* pBlop = writer.getBlop();
        sal_uInt32      aBlopSize = writer.getBlopSize();

        OSL_ENSURE(!key7.setValue(OUString(), RG_VALUETYPE_BINARY, (void*)pBlop, aBlopSize), "testCoreReflection error 9e");
        sal_uInt8* readBlop = (sal_uInt8*)rtl_allocateMemory(aBlopSize);
        OSL_ENSURE(!key7.getValue(OUString(), (void*)readBlop) , "testCoreReflection error 9e2");
    
        RegistryTypeReader reader(*pReaderLoader, readBlop, aBlopSize, sal_True);

        if (reader.isValid())
        {
            OSL_ENSURE(reader.getTypeName().equals(OUString::createFromAscii("ModuleA/ServiceA")), "testCoreReflection error 9e3");

            sal_uInt32 referenceCount = reader.getReferenceCount();
            OSL_ENSURE( referenceCount == 4, "testCoreReflection error 9e4");
            
            OUString refName = reader.getReferenceName(0);
            OSL_ENSURE(refName.equals(OUString::createFromAscii("ModuleA/XInterfaceA")), "testCoreReflection error 9e5");
        }	
    }

    {
        RegistryTypeWriter writer(*pWriterLoader,
                                  RT_TYPE_CONSTANTS,
                                  OUString::createFromAscii("ModuleA/ConstansA"),
                                  OUString(), 3, 0, 0);

        RTConstValue aConst;

        writer.setDoku(OUString::createFromAscii("Hallo ich bin eine Constants Group"));
        writer.setFileName(OUString::createFromAscii("DummyFile"));

        aConst.m_type = RT_TYPE_BOOL;
        aConst.m_value.aBool = sal_True;
        writer.setFieldData(0, OUString::createFromAscii("ConstantsA_aConstBool"), 
                            OUString::createFromAscii("boolean"), 
                            OUString::createFromAscii("ich bin ein boolean"), 
                            OUString::createFromAscii("DummyFile"), RT_ACCESS_CONST, aConst);
        aConst.m_type = RT_TYPE_BYTE;
        aConst.m_value.aByte = 127;
        writer.setFieldData(1, OUString::createFromAscii("ConstantsA_aConstByte"), 
                            OUString::createFromAscii("byte"), 
                            OUString::createFromAscii("ich bin ein byte"), 
                            OUString::createFromAscii("DummyFile"),  RT_ACCESS_CONST, aConst);
        aConst.m_type = RT_TYPE_INT16;
        aConst.m_value.aShort = -10;
        writer.setFieldData(2, OUString::createFromAscii("ConstantsA_aConstShort"), 
                            OUString::createFromAscii("short"), 
                            OUString::createFromAscii("ich bin ein short"), 
                            OUString::createFromAscii("DummyFile"),  RT_ACCESS_CONST, aConst);

        const sal_uInt8* pBlop = writer.getBlop();
        sal_uInt32      aBlopSize = writer.getBlopSize();

        OSL_ENSURE(!key8.setValue(OUString(), RG_VALUETYPE_BINARY, (void*)pBlop, aBlopSize), "testCoreReflection error 9f");
    }

    delete pWriterLoader;

//  OSL_ENSURE(!myRegistry->destroy(NULL), "testCoreReflection error 10");
    delete myRegistry;

    cout << "test_coreReflection() Ok!\n";
}

void test_registry_CppApi()
{
    RegistryLoader* pLoader = new RegistryLoader();

    if (!pLoader->isLoaded())
    {
        delete pLoader;
        return;
    }

    Registry *myRegistry = new Registry(*pLoader);
    delete pLoader;

    RegistryKey rootKey, key1, key2, key3, key4 ,key5, key6, key7, key8, key9;

    OSL_ENSURE(!myRegistry->create(OUString::createFromAscii("test.rdb")), "test_registry_CppApi error 1");
    OSL_ENSURE(!myRegistry->openRootKey(rootKey), "test_registry_CppApi error 2");

    OSL_ENSURE(!rootKey.createKey(OUString::createFromAscii("myFirstKey"), key1), "test_registry_CppApi error 3");
    OSL_ENSURE(!rootKey.createKey(OUString::createFromAscii("mySecondKey"), key2), "test_registry_CppApi error 4");
    OSL_ENSURE(!key1.createKey(OUString::createFromAscii("X"), key3), "test_registry_CppApi error 5");
    OSL_ENSURE(!key1.createKey(OUString::createFromAscii("mySecondSubKey"), key4), "test_registry_CppApi error 6");
    OSL_ENSURE(!rootKey.createKey(OUString::createFromAscii("myThirdKey"), key5), "test_registry_CppApi error 6a");

    OSL_ENSURE(!key5.createKey(OUString::createFromAscii("1"), key4), "test_registry_CppApi error 6b");
    OSL_ENSURE(!key4.createKey(OUString::createFromAscii("2"), key3), "test_registry_CppApi error 6c");
    OSL_ENSURE(!key5.openKey(OUString::createFromAscii("1"), key4), "test_registry_CppApi error 6d");
    OSL_ENSURE(!rootKey.openKey(OUString::createFromAscii("/myThirdKey/1"), key4), "test_registry_CppApi error 6e");
    OSL_ENSURE(key4.getName().equals(OUString::createFromAscii("/myThirdKey/1")), "test_registry_CppApi error 6f");

    OSL_ENSURE(!rootKey.createKey(OUString::createFromAscii("myFourthKey"), key6), "test_registry_CppApi error 7");
    OSL_ENSURE(!rootKey.createKey(OUString::createFromAscii("myFifthKey"), key6), "test_registry_CppApi error 7a");
    OSL_ENSURE(!rootKey.createKey(OUString::createFromAscii("mySixthKey"), key6), "test_registry_CppApi error 7b");

    // Link Test
    //

    OSL_ENSURE(!rootKey.createKey(OUString::createFromAscii("/myFourthKey/X"), key7), "test_registry_CppApi error 7c)");;
    OSL_ENSURE(!key6.createLink(OUString::createFromAscii("myFirstLink"), OUString::createFromAscii("/myFourthKey/X")), "test_registry_CppApi error 7d");
    OSL_ENSURE(!key6.createKey(OUString::createFromAscii("mySixthSubKey"), key7), "test_registry_CppApi error 7e");

    OUString linkTarget;
    OSL_ENSURE(!key6.getLinkTarget(OUString::createFromAscii("myFirstLink"), linkTarget), "test_registry_CppApi error 7f");
    OSL_ENSURE(linkTarget.equals(OUString::createFromAscii("/myFourthKey/X")), "test_registry_CppApi error 7g");

    RegistryKeyNames* pSubKeyNames = new RegistryKeyNames();
    sal_uInt32           nSubKeys=0;

    OSL_ENSURE(!rootKey.getKeyNames(OUString::createFromAscii("mySixthKey"), *pSubKeyNames), "test_registry_CppApi error 7h)");
    OSL_ENSURE(pSubKeyNames->getLength() == 2, "test_registry_CppApi error 7i)");

    for (sal_uInt32 i=0; i < pSubKeyNames->getLength(); i++)
    {
        if (pSubKeyNames->getElement(i).equals(OUString::createFromAscii("/mySixthKey/myFirstLink")))
        {
            RegKeyType keyType;
            OSL_ENSURE(!rootKey.getKeyType(pSubKeyNames->getElement(i), &keyType), "test_registry_CppApi error 7j");
            OSL_ENSURE(keyType == RG_LINKTYPE, "test_registry_CppApi error 7k");
        }
    }

    OSL_ENSURE(!key7.closeKey(), "test_registry_CppApi error 7k1");
    delete pSubKeyNames;

    OSL_ENSURE(!rootKey.openKey(OUString::createFromAscii("/mySixthKey/myFirstLink"), key6), "test_registry_CppApi error 7l");
    OSL_ENSURE(key6.getName().equals(OUString::createFromAscii("/myFourthKey/X")), "test_registry_CppApi error 7m");

    OSL_ENSURE(!rootKey.openKey(OUString::createFromAscii("myFifthKey"), key6), "test_registry_CppApi error 7m1");
    OSL_ENSURE(!key6.createLink(OUString::createFromAscii("mySecondLink"), 
                                 OUString::createFromAscii("/mySixthKey/myFirstLink")), "test_registry_CppApi error 7m2");

    OSL_ENSURE(!rootKey.openKey(OUString::createFromAscii("/myFifthKey/mySecondLink"), key6), "test_registry_CppApi error 7m3");
    OSL_ENSURE(key6.getName().equals(OUString::createFromAscii("/myFourthKey/X")), "test_registry_CppApi error 7m4");

    OSL_ENSURE(!rootKey.createKey(OUString::createFromAscii("/myFifthKey/mySecondLink/myFirstLinkSubKey"), key7), "test_registry_CppApi error 7m5");
    OSL_ENSURE(key7.getName().equals(OUString::createFromAscii("/myFourthKey/X/myFirstLinkSubKey")), "test_registry_CppApi error 7m6");

    OSL_ENSURE(!key7.createLink(OUString::createFromAscii("myThirdLink"), OUString::createFromAscii("/myFifthKey/mySecondLink")), "test_registry_CppApi error 7m7");
    OSL_ENSURE(!rootKey.openKey(OUString::createFromAscii("/myFourthKey/X/myFirstLinkSubKey/myThirdLink"), key7), "test_registry_CppApi error 7m8");
    OSL_ENSURE(!key7.openKey(OUString::createFromAscii("/myFirstLinkSubKey/myThirdLink/myFirstLinkSubKey/myThirdLink"), key6), "test_registry_CppApi error 7m9");
    OSL_ENSURE(key7.getName().equals(OUString::createFromAscii("/myFourthKey/X")), "test_registry_CppApi error 7m10");
    OSL_ENSURE(!key7.closeKey(), "test_registry_CppApi error 7m11");

    OSL_ENSURE(!rootKey.deleteLink(OUString::createFromAscii("/myFifthKey/mySecondLink")), "test_registry_CppApi error 7m12");

    OSL_ENSURE(!rootKey.createLink(OUString::createFromAscii("/myFifthKey/mySecondLink"),
                                    OUString::createFromAscii("/myFourthKey/X/myFirstLinkSubKey/myThirdLink")),
                                    "test_registry_CppApi error 7m13");

    OSL_ENSURE(rootKey.openKey(OUString::createFromAscii("/myFourthKey/X/myFirstLinkSubKey/myThirdLink"), key7) == REG_DETECT_RECURSION,
                "test_registry_CppApi error 7m14");

    OSL_ENSURE(key7.closeKey() == REG_INVALID_KEY, "test_registry_CppApi error 7m11");

    RegistryKeyNames subKeyNames;
    nSubKeys=0;

    OSL_ENSURE(!rootKey.getKeyNames(OUString::createFromAscii("mySixthKey"), subKeyNames), "test_registry_CppApi error 7n");

    nSubKeys = subKeyNames.getLength();
    OSL_ENSURE(nSubKeys == 2, "test_registry_CppApi error 7n1");
    OSL_ENSURE(subKeyNames.getElement(0).equals(OUString::createFromAscii("/mySixthKey/myFirstLink")), "test_registry_CppApi error 7p1)");
    OSL_ENSURE(subKeyNames.getElement(1).equals(OUString::createFromAscii("/mySixthKey/mySixthSubKey")), "test_registry_CppApi error 7p2");


    RegistryKeyArray subKeys;
    nSubKeys=0;

    OSL_ENSURE(!rootKey.openSubKeys(OUString::createFromAscii("myFirstKey"), subKeys), "test_registry_CppApi error 7o");

    nSubKeys = subKeys.getLength();
    OSL_ENSURE(nSubKeys == 2, "test_registry_CppApi error 7o1");
    OSL_ENSURE(subKeys.getElement(0).getName().equals(OUString::createFromAscii("/myFirstKey/mySecondSubKey")), "test_registry_CppApi error 7p1)");
    OSL_ENSURE(subKeys.getElement(1).getName().equals(OUString::createFromAscii("/myFirstKey/X")), "test_registry_CppApi error 7p2");

    OSL_ENSURE(!rootKey.closeSubKeys(subKeys), "test_registry_CppApi error 7q)");


    OSL_ENSURE(!rootKey.createKey(OUString::createFromAscii("/TEST"), key8), "test_registry_CppApi error 8");
    OSL_ENSURE(!rootKey.createKey(OUString::createFromAscii("/TEST/Child1"), key8), "test_registry_CppApi error 8a");
    OSL_ENSURE(!rootKey.createKey(OUString::createFromAscii("/TEST/Child2"), key8), "test_registry_CppApi error 8a1");
    OSL_ENSURE(!rootKey.openKey(OUString::createFromAscii("/TEST"), key9), "test_registry_CppApi error 8b");
    OSL_ENSURE(!key8.closeKey() && !key9.closeKey(),  "test_registry_CppApi error 8b1");
    OSL_ENSURE(!rootKey.openKey(OUString::createFromAscii("/TEST"), key8), "test_registry_CppApi error 8b");
    OSL_ENSURE(!key8.closeKey(),  "test_registry_CppApi error 8c");
    OSL_ENSURE(!rootKey.openKey(OUString::createFromAscii("TEST"), key8), "test_registry_CppApi error 8c");
    OSL_ENSURE(!key8.closeKey(),  "test_registry_CppApi error 8d");


    sal_Char* Value="Mein erster Value";
    OSL_ENSURE(!rootKey.setValue(OUString::createFromAscii("mySecondKey"), RG_VALUETYPE_STRING, Value, 18), "test_registry_CppApi error 9");

    RegValueType    valueType;
    sal_uInt32          valueSize;
    sal_Char*           readValue;
    OSL_ENSURE(!rootKey.getValueInfo(OUString::createFromAscii("mySecondKey"), &valueType, &valueSize), "test_registry_CppApi error 9a");

    readValue = (sal_Char*)rtl_allocateMemory(valueSize);
    OSL_ENSURE(!key2.getValue(OUString(), readValue), "test_registry_CppApi error 10");

    OSL_ENSURE(valueType == RG_VALUETYPE_STRING, "test_registry_CppApi error 11");
    OSL_ENSURE(valueSize == 18, "test_registry_CppApi error 12");
    OSL_ENSURE(strcmp(readValue, Value) == 0, "test_registry_CppApi error 13");
    rtl_freeMemory(readValue);

    const sal_Char* pList[3];
    const sal_Char* n1= "Hallo";
    const sal_Char* n2= "jetzt komm";
    const sal_Char* n3= "ich";

    pList[0]=n1;
    pList[1]=n2;
    pList[2]=n3;

    OSL_ENSURE(!rootKey.setStringListValue(OUString::createFromAscii("myFourthKey"), (sal_Char**)pList, 3), "test_registry_CppApi error 13a");

    RegistryValueList<sal_Char*> valueList;
    OSL_ENSURE(!rootKey.getStringListValue(OUString::createFromAscii("myFourthKey"), valueList), "test_registry_CppApi error 13b");

    OSL_ENSURE(strcmp(n1, valueList.getElement(0)) == 0, "test_registry_CppApi error 13c");
    OSL_ENSURE(strcmp(n2, valueList.getElement(1)) == 0, "test_registry_CppApi error 13d");
    OSL_ENSURE(strcmp(n3, valueList.getElement(2)) == 0, "test_registry_CppApi error 13e");

    OSL_ENSURE(!rootKey.getValueInfo(OUString::createFromAscii("myFourthKey"), &valueType, &valueSize), "test_registry_CppApi error 13e1");
    OSL_ENSURE(valueType == RG_VALUETYPE_STRINGLIST, "test_registry_CppApi error 13e2");
    OSL_ENSURE(valueSize == 3, "test_registry_CppApi error 13e3");

    sal_Int32 pLong[3];
    pLong[0] = 123;
    pLong[1] = 456;
    pLong[2] = 789;

    OSL_ENSURE(!rootKey.setLongListValue(OUString::createFromAscii("myFifthKey"), pLong, 3), "test_registry_CppApi error 13f");

    RegistryValueList<sal_Int32> longList;
    OSL_ENSURE(!rootKey.getLongListValue(OUString::createFromAscii("myFifthKey"), longList), "test_registry_CppApi error 13g");

    OSL_ENSURE(pLong[0] == longList.getElement(0), "test_registry_CppApi error 13h");
    OSL_ENSURE(pLong[1] == longList.getElement(1), "test_registry_CppApi error 13i");
    OSL_ENSURE(pLong[2] == longList.getElement(2), "test_registry_CppApi error 13j");


    OUString sWTestValue(OUString::createFromAscii( "Mein erster Unicode Value" ));
    const sal_Unicode* wTestValue= sWTestValue.getStr();
    OSL_ENSURE(!rootKey.setValue(OUString::createFromAscii("mySixthKey"), RG_VALUETYPE_UNICODE, (void*)wTestValue,
                (rtl_ustr_getLength(wTestValue)+1)*sizeof(sal_Unicode)), "test_registry_CppApi error 13j1");

    OSL_ENSURE(!rootKey.getValueInfo(OUString::createFromAscii("mySixthKey"), &valueType, &valueSize), "test_registry_CppApi error 13j2");
    sal_Unicode* pTmpValue = (sal_Unicode*)rtl_allocateMemory(valueSize);
    OSL_ENSURE(!rootKey.getValue(OUString::createFromAscii("mySixthKey"), pTmpValue), "test_registry_CppApi error 13j3");
    OSL_ENSURE(rtl_ustr_getLength(wTestValue) == rtl_ustr_getLength(pTmpValue), "test_registry_CppApi error 13j4");
    OSL_ENSURE(rtl_ustr_compare(wTestValue, pTmpValue) == 0, "test_registry_CppApi error 13j4");

    const sal_Unicode* pUnicode[3];
    OUString w1(OUString::createFromAscii( "Hallo" ));
    OUString w2(OUString::createFromAscii( "jetzt komm" ));
    OUString w3(OUString::createFromAscii( "ich als unicode" ));
    
    pUnicode[0]=w1.getStr();
    pUnicode[1]=w2.getStr();
    pUnicode[2]=w3.getStr();

    OSL_ENSURE(!rootKey.setUnicodeListValue(OUString::createFromAscii("mySixthKey"), (sal_Unicode**)pUnicode, 3), "test_registry_CppApi error 13k");

    RegistryValueList<sal_Unicode*> unicodeList;
    OSL_ENSURE(!rootKey.getUnicodeListValue(OUString::createFromAscii("mySixthKey"), unicodeList), "test_registry_CppApi error 13l");

    OSL_ENSURE(rtl_ustr_compare(w1, unicodeList.getElement(0)) == 0, "test_registry_CppApi error 13m");
    OSL_ENSURE(rtl_ustr_compare(w2, unicodeList.getElement(1)) == 0, "test_registry_CppApi error 13n");
    OSL_ENSURE(rtl_ustr_compare(w3, unicodeList.getElement(2)) == 0, "test_registry_CppApi error 13o");

    OSL_ENSURE(!key6.closeKey(),  "test_registry_CppApi error 14");

    OSL_ENSURE(!key1.closeKey() &&
               !key3.closeKey() &&
               !key4.closeKey(),  "test_registry_CppApi error 14");

    OSL_ENSURE(!rootKey.deleteKey(OUString::createFromAscii("myFirstKey")), "test_registry_CppApi error 15");

    OSL_ENSURE(!key2.closeKey(), "test_registry_CppApi error 16");
    OSL_ENSURE(!rootKey.openKey(OUString::createFromAscii("mySecondKey"), key2), "test_registry_CppApi error 17");

    OSL_ENSURE(!key5.closeKey(), "test_registry_CppApi error 18");

    OSL_ENSURE(!rootKey.deleteKey(OUString::createFromAscii("myThirdKey")), "test_registry_CppApi error 19");

    OSL_ENSURE(rootKey.openKey(OUString::createFromAscii("myThirdKey"), key5), "test_registry_CppApi error 20");

    OSL_ENSURE(!key2.closeKey() &&
                !rootKey.closeKey(),  "test_registry_CppApi error 21");

    OSL_ENSURE(!myRegistry->close(), "test_registry_CppApi error 22");

    // Test loadkey
    RegistryKey rootKey2, key21, key22, key23, key24 , key25;

    OSL_ENSURE(!myRegistry->create(OUString::createFromAscii("test2.rdb")), "test_registry_CppApi error 23");
    OSL_ENSURE(!myRegistry->openRootKey(rootKey2), "test_registry_CppApi error 24");

    OSL_ENSURE(!rootKey2.createKey(OUString::createFromAscii("reg2FirstKey"), key21), "test_registry_CppApi error 25");
    OSL_ENSURE(!rootKey2.createKey(OUString::createFromAscii("reg2SecondKey"), key22), "test_registry_CppApi error 26");
    OSL_ENSURE(!key21.createKey(OUString::createFromAscii("reg2FirstSubKey"), key23), "test_registry_CppApi error 27");
    OSL_ENSURE(!key21.createKey(OUString::createFromAscii("reg2SecondSubKey"), key24), "test_registry_CppApi error 28");
    OSL_ENSURE(!rootKey2.createKey(OUString::createFromAscii("reg2ThirdKey"), key25), "test_registry_CppApi error 29");

    sal_uInt32 nValue= 123456789;
    OSL_ENSURE(!key23.setValue(OUString(), RG_VALUETYPE_LONG, &nValue, sizeof(sal_uInt32)), "test_registry_CppApi error 30");

    OSL_ENSURE(!key21.closeKey() &&
               !key22.closeKey() &&
               !key23.closeKey() &&
               !key24.closeKey() &&
               !key25.closeKey() &&
               !rootKey2.closeKey(), "test_registry_CppApi error 31");

    OSL_ENSURE(!myRegistry->close(), "test_registry_CppApi error 32");

    OSL_ENSURE(!myRegistry->open(OUString::createFromAscii("test.rdb"), REG_READWRITE), "test_registry_CppApi error 33");
    OSL_ENSURE(!myRegistry->openRootKey(rootKey), "test_registry_CppApi error 34");

    OSL_ENSURE(!myRegistry->loadKey(rootKey, OUString::createFromAscii("allFromTest2"), 
                    OUString::createFromAscii("test2.rdb")), "test_registry_CppApi error 35");
    OSL_ENSURE(!myRegistry->saveKey(rootKey, OUString::createFromAscii("allFromTest2"), 
                    OUString::createFromAscii("test3.rdb")), "test_registry_CppApi error 36");

    OSL_ENSURE(!rootKey.createKey(OUString::createFromAscii("allFromTest3"), key1), "test_registry_CppApi error 37");
    OSL_ENSURE(!key1.createKey(OUString::createFromAscii("myFirstKey2"), key2), "test_registry_CppApi error 38");
    OSL_ENSURE(!key1.createKey(OUString::createFromAscii("mySecondKey2"), key3), "test_registry_CppApi error 39");

    OSL_ENSURE(!myRegistry->mergeKey(rootKey, OUString::createFromAscii("allFromTest3"), 
                    OUString::createFromAscii("test3.rdb")), "test_registry_CppApi error 40");
    OSL_ENSURE(!myRegistry->mergeKey(rootKey, OUString::createFromAscii("allFromTest3"), 
                    OUString::createFromAscii("ucrtest.rdb"), sal_True), "test_registry_CppApi error 40.a)");

    OSL_ENSURE(myRegistry->mergeKey(rootKey, OUString::createFromAscii("allFromTest3"), OUString::createFromAscii("ucrtest.rdb"), sal_True)
                 == REG_NO_ERROR/*REG_MERGE_CONFLICT*/, "test_registry_CppApi error 40.b)");

    OSL_ENSURE(!key1.closeKey() &&
                !key2.closeKey(), "test_registry_CppApi error 41");

    const sal_Unicode* wValue= OUString::createFromAscii( "Mein erster Unicode Value" ).getStr();
    OSL_ENSURE(!key3.setValue(OUString(), RG_VALUETYPE_UNICODE, (void*)wValue,
                (rtl_ustr_getLength(wValue)+1)*sizeof(sal_Unicode)), "test_registry_CppApi error 42");

    OSL_ENSURE(!key3.closeKey(), "test_registry_CppApi error 43");

    OSL_ENSURE(!rootKey.openKey(OUString::createFromAscii("/allFromTest3/reg2FirstKey/reg2FirstSubKey"), key1),
                "test_registry_CppApi error 43.a)");
    OSL_ENSURE(!rootKey.deleteKey(OUString::createFromAscii("/allFromTest3/reg2FirstKey/reg2FirstSubKey")), "test_registry_CppApi error 44");
    OSL_ENSURE(key1.getValueInfo(OUString(), &valueType, &valueSize) == REG_INVALID_KEY,
                "test_registry_CppApi error 44.a)");
    OSL_ENSURE(!key1.closeKey(), "test_registry_CppApi error 44.b)");

    OSL_ENSURE(!rootKey.closeKey(), "test_registry_CppApi error 45");

    OSL_ENSURE(!myRegistry->close(), "test_registry_CppApi error 46");

    OSL_ENSURE(!myRegistry->open(OUString::createFromAscii("test.rdb"), REG_READWRITE), "test_registry_CppApi error 47");

    OSL_ENSURE(!myRegistry->destroy(OUString::createFromAscii("test2.rdb")), "test_registry_CppApi error 48");
//  OSL_ENSURE(!myRegistry->destroy("test3.rdb"), "test_registry_CppApi error 49");

    Registry *myRegistry2 = new Registry(*myRegistry);

    OSL_ENSURE(myRegistry->destroy(OUString()), "test_registry_CppApi error 50");

    delete(myRegistry2);

    OSL_ENSURE(!myRegistry->create(OUString::createFromAscii("destroytest.rdb")), "test_registry_CppApi error 51");
    OSL_ENSURE(!myRegistry->close(), "test_registry_CppApi error 52");
    OSL_ENSURE(!myRegistry->open(OUString::createFromAscii("destroytest.rdb"), REG_READONLY), "test_registry_CppApi error 53");
    OSL_ENSURE(!myRegistry->openRootKey(rootKey), "test_registry_CppApi error 54");

    OSL_ENSURE(myRegistry->mergeKey(rootKey, OUString::createFromAscii("allFromTest3"), 
                    OUString::createFromAscii("test3.rdb")), "test_registry_CppApi error 55");
    OSL_ENSURE(!myRegistry->destroy(OUString::createFromAscii("test3.rdb")), "test_registry_CppApi error 56");

    OSL_ENSURE(!rootKey.closeKey(), "test_registry_CppApi error 57");
    OSL_ENSURE(!myRegistry->close(), "test_registry_CppApi error 58");
    OSL_ENSURE(!myRegistry->open(OUString::createFromAscii("destroytest.rdb"), REG_READWRITE), "test_registry_CppApi error 59");
    OSL_ENSURE(!myRegistry->destroy(OUString()), "test_registry_CppApi error 60");

    OSL_ENSURE(!myRegistry->open(OUString::createFromAscii("test.rdb"), REG_READWRITE), "test_registry_CppApi error 61");
    OSL_ENSURE(!myRegistry->destroy(OUString::createFromAscii("ucrtest.rdb")), "test_registry_CppApi error 62");
    OSL_ENSURE(!myRegistry->destroy(OUString()), "test_registry_CppApi error 63");
    delete(myRegistry);

    cout << "test_registry_CppApi() Ok!\n";

    return;
}


