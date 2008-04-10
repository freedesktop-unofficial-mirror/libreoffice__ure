/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: ByteSequence.cxx,v $
 * $Revision: 1.13 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/


// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sal.hxx"
#include <Byte_Const.h>
#include <rtl/byteseq.h>

#include <rtl/byteseq.hxx>

#include <cppunit/simpleheader.hxx>

using namespace rtl;

namespace rtl_ByteSequence
{

//------------------------------------------------------------------------
// testing constructors
//------------------------------------------------------------------------

class  ctor : public CppUnit::TestFixture
    {
    public:
        
        void ctor_001()
        {
            ::rtl::ByteSequence aByteSeq1;
        ::rtl::ByteSequence aByteSeq2( &kTestEmptyByteSeq );
            CPPUNIT_ASSERT_MESSAGE
            (
                "Creates an empty sequence",
                aByteSeq1.getLength() == 0 && 
                aByteSeq1 == aByteSeq2
            );
        }
        
        void ctor_002()
        {
            ::rtl::ByteSequence aByteSeq;
            ::rtl::ByteSequence aByteSeqtmp( aByteSeq );
            CPPUNIT_ASSERT_MESSAGE
            (
                "Creates a copy of given sequence",
                aByteSeq == aByteSeqtmp 
            );
         
        }
        
        void ctor_003()
        {
            ::rtl::ByteSequence aByteSeq( &kTestByteSeq1 );
        sal_Int32 nNewLen = aByteSeq.getLength();
            CPPUNIT_ASSERT_MESSAGE
            (
                "Copy constructor Creates a copy from the C-Handle ",
                nNewLen == kTestSeqLen1
        );
        }
    
    void ctor_003_1()
        {
            ::rtl::ByteSequence aByteSeq( &kTestByteSeq2 );
        sal_Int32 nNewLen = aByteSeq.getLength();
            CPPUNIT_ASSERT_MESSAGE
            (
                "Copy constructor Creates a copy from the C-Handle: reference count > 1 ",
                nNewLen == kTestSeqLen2 
        );
        }

        void ctor_004()
        {
            sal_Int8 * pElements = &kTestByte4;
        sal_Int32 len = kTestByteCount1;
        ::rtl::ByteSequence aByteSeq( pElements, len);
        sal_Int32 nNewLen = aByteSeq.getLength();
            CPPUNIT_ASSERT_MESSAGE
            (
                "Creates a copy of given data bytes",
                aByteSeq[1] == pElements[1] &&
                len == nNewLen
                         
            );
        }
        
    void ctor_005()
        {
        sal_Int32 len = 50;
            ::rtl::ByteSequence aByteSeq( len );
        sal_Int32 nNewLen = aByteSeq.getLength();
        sal_Bool res = sal_True;
        for (sal_Int32 i=0; i<len; i++)
        {
                if (aByteSeq[i] != 0)
                    res = sal_False;
            }
            CPPUNIT_ASSERT_MESSAGE
            (
                "Creates sequence of given length and initializes all bytes to 0",
                nNewLen == len && res
                
            );
        }

        void ctor_006()
        {
        sal_Int32 len = 39;
            ::rtl::ByteSequence aByteSeq( len , BYTESEQ_NODEFAULT );
            sal_Int32 nNewLen = aByteSeq.getLength();
            CPPUNIT_ASSERT_MESSAGE
            (
                "Creates sequence of given length and does NOT initialize data",
                nNewLen == len

            );
        }

        void ctor_007()
        {
            ::rtl::ByteSequence aByteSeq( &kTestByteSeq3, BYTESEQ_NOACQUIRE );
        sal_Int32 nNewLen = aByteSeq.getLength();
            CPPUNIT_ASSERT_MESSAGE
            (
                "Creates a sequence from a C-Handle without acquiring the handle, thus taking over ownership",
                nNewLen == kTestSeqLen3		            
            );
        }
        
        CPPUNIT_TEST_SUITE(ctor);
        CPPUNIT_TEST(ctor_001);
        CPPUNIT_TEST(ctor_002);
        CPPUNIT_TEST(ctor_003);
    CPPUNIT_TEST(ctor_003_1);
        CPPUNIT_TEST(ctor_004);
        CPPUNIT_TEST(ctor_005);
        CPPUNIT_TEST(ctor_006);
        CPPUNIT_TEST(ctor_007);
        CPPUNIT_TEST_SUITE_END();
    };

class assign : public CppUnit::TestFixture
{
public:
    // initialise your test code values here.
    void setUp()
    {
    }

    void tearDown()
    {
    }

    // insert your test code here.
    void assign_001()
    {
    sal_Int32 len = kTestByteCount1;
    sal_Int32 len2 = len - 1;
    sal_Int8 * pElements = &kTestByte;
    ::rtl::ByteSequence aByteSeq1( kTestByte5, len);
    ::rtl::ByteSequence aByteSeq2( pElements, len2);
    aByteSeq2 = aByteSeq1;
    sal_Int32 nNewLen = aByteSeq2.getLength();
        CPPUNIT_ASSERT_MESSAGE
        (   
            "Assignment operator: assign longer sequence to another",
            aByteSeq1 == aByteSeq2 && 
            nNewLen == len	    
        );
    }

    void assign_002()
    {
        sal_Int32 len = kTestByteCount1 - 1 ;
        ::rtl::ByteSequence aByteSeq1( len );
    sal_Int8 * pElements = &kTestByte1;
    ::rtl::ByteSequence aByteSeq2( pElements, len + 1);
    aByteSeq2 = aByteSeq1;
    sal_Int32 nNewLen = aByteSeq2.getLength();
        CPPUNIT_ASSERT_MESSAGE
        (   
            "Assignment operator: assign shorter sequence to another",
            aByteSeq1 == aByteSeq2 && 
            nNewLen == len
        );
    }
    
    void assign_003()
    {
        sal_Int32 len = kTestByteCount1 - 1 ;
    const sal_Int8 * pElements = &kTestByte2;
        ::rtl::ByteSequence aByteSeq1( pElements, len + 1 );
    ::rtl::ByteSequence aByteSeq2( len, BYTESEQ_NODEFAULT );
    aByteSeq2 = aByteSeq1;
    sal_Int32 nNewLen = aByteSeq2.getLength();
        CPPUNIT_ASSERT_MESSAGE
        (   
            "Assignment operator: assign sequence to another sequence having no data initialized",
            aByteSeq1 == aByteSeq2 && 
            nNewLen == kTestByteCount1	    
        );
    }

    void assign_004()
    {
        ::rtl::ByteSequence aByteSeq1;
    sal_Int32 len = kTestByteCount1 ;
    const sal_Int8 * pElements = &kTestByte;
    ::rtl::ByteSequence aByteSeq2( pElements, len);
    aByteSeq2 = aByteSeq1;
    sal_Int32 nNewLen = aByteSeq2.getLength();
        CPPUNIT_ASSERT_MESSAGE
        (   
            "Assignment operator: assign empty sequence to another not empty sequence",
            aByteSeq1 == aByteSeq2 && 
        nNewLen == 0	    
        );
    }

    CPPUNIT_TEST_SUITE(assign);
    CPPUNIT_TEST(assign_001);
    CPPUNIT_TEST(assign_002);
    CPPUNIT_TEST(assign_003);
    CPPUNIT_TEST(assign_004);
    CPPUNIT_TEST_SUITE_END();
}; // class operator=


class equal : public CppUnit::TestFixture
{
public:
    // initialise your test code values here.
    void setUp()
    {
    }

    void tearDown()
    {
    }

    // insert your test code here.
    void equal_001()
    {
    sal_Int32 len = kTestByteCount1 ;
        sal_Int8 * pElements = &kTestByte;
    ::rtl::ByteSequence aByteSeq1( pElements, len-1);
    ::rtl::ByteSequence aByteSeq2( pElements, len);
    sal_Bool res = aByteSeq1 == aByteSeq2;
        CPPUNIT_ASSERT_MESSAGE
        (   
            "Equality operator: compare two sequences 1",
            !res
        );
    }

    void equal_002()
    {
        sal_Int32 len = kTestByteCount1 ;
        const sal_Int8 * pElements = &kTestByte;
    ::rtl::ByteSequence aByteSeq1( pElements, len);
    ::rtl::ByteSequence aByteSeq2( pElements, len);
    sal_Bool res = aByteSeq1 == aByteSeq2;
        CPPUNIT_ASSERT_MESSAGE
        (   
            "Equality operator: compare two sequences 2",
            res
        );
    }

    void equal_003()
    {
        sal_Int32 len = kTestByteCount1 ;
        const sal_Int8 * pElements = kTestByte5;
    ::rtl::ByteSequence aByteSeq1( pElements, len);
    pElements = kTestByte6;
    ::rtl::ByteSequence aByteSeq2( pElements, len);
    sal_Bool res = aByteSeq1 == aByteSeq2;
        CPPUNIT_ASSERT_MESSAGE
        (   
            "Equality operator: compare two sequences 2",
            !res
        );
    }
    CPPUNIT_TEST_SUITE(equal);
    CPPUNIT_TEST(equal_001);
    CPPUNIT_TEST(equal_002);
    CPPUNIT_TEST(equal_003);
    CPPUNIT_TEST_SUITE_END();
}; // class equal


class notequal : public CppUnit::TestFixture
{
public:
    // initialise your test code values here.
    void setUp()
    {
    }

    void tearDown()
    {
    }

    // insert your test code here.
    void notequal_001()
    {
        sal_Int32 len = kTestByteCount1 ;
        const sal_Int8 * pElements = &kTestByte;
    ::rtl::ByteSequence aByteSeq1( pElements, len-1);
    ::rtl::ByteSequence aByteSeq2( pElements, len);
    sal_Bool res = aByteSeq1 != aByteSeq2;
        CPPUNIT_ASSERT_MESSAGE
        (   
            "Equality operator: compare two sequences 1",
            res
        );
    }

    void notequal_002()
    {
        sal_Int32 len = kTestByteCount1 ;
        const sal_Int8 * pElements = &kTestByte;
    ::rtl::ByteSequence aByteSeq1( pElements, len);
    ::rtl::ByteSequence aByteSeq2( pElements, len);
    sal_Bool res = aByteSeq1 != aByteSeq2;
        CPPUNIT_ASSERT_MESSAGE
        (   
            "Equality operator: compare two sequences 2",
            !res
        );
    }

    CPPUNIT_TEST_SUITE(notequal);
    CPPUNIT_TEST(notequal_001);
    CPPUNIT_TEST(notequal_002);
    CPPUNIT_TEST_SUITE_END();
}; // class notequal


class getArray : public CppUnit::TestFixture
{
public:
    // initialise your test code values here.
    void setUp()
    {
    }

    void tearDown()
    {
    }

    // insert your test code here.
    void getArray_001()
    {
        sal_Int32 len = kTestByteCount1 ;
        sal_Int8 * pElements = &kTestByte;
    ::rtl::ByteSequence aByteSeq1( pElements, len);
    sal_Int8 * pArray = aByteSeq1.getArray();
    sal_Bool res = sal_True;
    for (sal_Int32 i = 0; i < len; i++)
    {
        if (pElements[i] != pArray[i])
        res = sal_False;
    }
    CPPUNIT_ASSERT_MESSAGE
        (   
            "Gets the pointer to byte array: one element sequence",
        res == sal_True
        );
    }

    void getArray_002()
    {
    sal_Int8 * pElements = kTestByte6;
        ::rtl::ByteSequence aByteSeq(pElements, 5);
    sal_Int8 * pArray = aByteSeq.getArray();
    sal_Bool res = sal_True;
    for (sal_Int32 i = 0; i < 5; i++)
    {
        if (pElements[i] != pArray[i])
        res = sal_False;
    }
    CPPUNIT_ASSERT_MESSAGE
        (        
            "Gets the pointer to byte array: more elements sequence",
            res == sal_True
        );
    }

    CPPUNIT_TEST_SUITE(getArray);
    CPPUNIT_TEST(getArray_001);
    CPPUNIT_TEST(getArray_002);
    CPPUNIT_TEST_SUITE_END();
}; // class getArray


class realloc : public CppUnit::TestFixture
{
public:
    // initialise your test code values here.
    void setUp()
    {
    }

    void tearDown()
    {
    }

    // insert your test code here.
    void realloc_001()
    {
        ::rtl::ByteSequence aByteSeq;
    sal_Int32 nSize = 20;
    aByteSeq.realloc( nSize );
    sal_Int32 nNewLen = aByteSeq.getLength();
        CPPUNIT_ASSERT_MESSAGE
        (        
            "Reallocates sequence to new length: empty sequence",
            nNewLen == nSize 
        );
    }

    void realloc_002()
    {
    //reference count > 1
    ::rtl::ByteSequence aByteSeq( &kTestByteSeq2 );  //34
    sal_Int32 nSize = 20;
    aByteSeq.realloc( nSize );
    sal_Int32 nNewLen = aByteSeq.getLength();
    CPPUNIT_ASSERT_MESSAGE
        (        
            "Reallocates sequence: reference count > 1 && nSize < nElements",
            nNewLen == nSize 
        );
    }
    
    void realloc_003()
    {
    //reference count > 1
    ::rtl::ByteSequence aByteSeq( &kTestByteSeq2 );  //34
    sal_Int32 nSize = kTestSeqLen2 + 5;
    sal_Int32 nElements = kTestSeqLen2;
    aByteSeq.realloc( nSize );
    sal_Int32 nNewLen = aByteSeq.getLength();
    sal_Bool res = sal_True;
    for (int i = nSize; i < nElements; i++)
    {
            sal_Int8 nValue = aByteSeq[i];
        if (nValue != 0)
                res = sal_False;
        }
    CPPUNIT_ASSERT_MESSAGE
        (        
            "Reallocates sequence: reference count > 1 && nSize > nElements",
            nNewLen == nSize 
        && res == sal_True
        );
    }

    void realloc_004()
    {
    sal_Int8 * pElements = &kTestByte3;
    sal_Int32 len = kTestByteCount3;
    ::rtl::ByteSequence aByteSeq( pElements, len);
    sal_Int32 nSize = kTestByteCount3 - 10 ;
    aByteSeq.realloc( nSize );
    sal_Int32 nNewLen = aByteSeq.getLength();
    CPPUNIT_ASSERT_MESSAGE
        (        
            "Reallocates sequence: nSize < nElements",
            nNewLen == nSize 
        );
    }
    
    void realloc_005()
    {
    sal_Int8 * pElements = kTestByte6;
    sal_Int32 len = 4;
    ::rtl::ByteSequence aByteSeq( pElements, len);
    sal_Int32 nSize = len + 10 ;
    aByteSeq.realloc( nSize );
    sal_Int32 nNewLen = aByteSeq.getLength();
    CPPUNIT_ASSERT_MESSAGE
        (        
            "Reallocates sequence: nSize > nElements",
            nNewLen == nSize 
        );
    }
    
    CPPUNIT_TEST_SUITE(realloc);
    CPPUNIT_TEST(realloc_001);
    CPPUNIT_TEST(realloc_002);
    CPPUNIT_TEST(realloc_003);
    CPPUNIT_TEST(realloc_004);
    CPPUNIT_TEST(realloc_005);
    //CPPUNIT_TEST(realloc_006);
    CPPUNIT_TEST_SUITE_END();
}; // class realloc


class getData : public CppUnit::TestFixture
{
public:
    // initialise your test code values here.
    void setUp()
    {
    }

    void tearDown()
    {
    }

    // insert your test code here.
    void getData_001()
    {
    sal_Int8 * pElements = kTestByte5;
        ::rtl::ByteSequence aByteSeq(pElements, 4);
    sal_Bool res = sal_True;
    if (aByteSeq[0] != kTestByte)
            res = sal_False;
    
        if (aByteSeq[1] != kTestByte1)
            res = sal_False;
      
    if (aByteSeq[2] != kTestByte2)
            res = sal_False;

    if (aByteSeq[3] != kTestByte3)
            res = sal_False;

    CPPUNIT_ASSERT_MESSAGE
        (        
            "Obtains a reference to byte indexed at given position: empty sequence",
            res == sal_True
        );
    }

    void getData_002()
    {
        ::rtl::ByteSequence aByteSeq( &kTestByteSeq2 );
    sal_Int8 nValue = aByteSeq[0];
        CPPUNIT_ASSERT_MESSAGE
        (
            "Obtains a reference to byte indexed at given position: reference count > 1",
            nValue == kTestChar2 //not sure what is right,hehe
    );
    }

    void getData_003()
    {
        ::rtl::ByteSequence aByteSeq( &kTestByteSeq3 );
    sal_Int8 nValue = aByteSeq[0];
        CPPUNIT_ASSERT_MESSAGE
        (
            "Obtains a reference to byte indexed at given position: reference count = 1",
            nValue == kTestChar3 //not sure what is right,hehe
    );
    }

    CPPUNIT_TEST_SUITE(getData);
    CPPUNIT_TEST(getData_001);
    CPPUNIT_TEST(getData_002);
    CPPUNIT_TEST(getData_003);
    CPPUNIT_TEST_SUITE_END();
}; // class getData

// -----------------------------------------------------------------------------
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(rtl_ByteSequence::ctor, "rtl_ByteSequence");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(rtl_ByteSequence::assign, "rtl_ByteSequence");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(rtl_ByteSequence::equal, "rtl_ByteSequence");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(rtl_ByteSequence::notequal, "rtl_ByteSequence");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(rtl_ByteSequence::getArray, "rtl_ByteSequence");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(rtl_ByteSequence::realloc, "rtl_ByteSequence");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(rtl_ByteSequence::getData, "rtl_ByteSequence");
} // namespace ByteSequence


// -----------------------------------------------------------------------------

// this macro creates an empty function, which will called by the RegisterAllFunctions()
// to let the user the possibility to also register some functions by hand.
NOADDITIONAL;
