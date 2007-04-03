/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: rtl_OUString2.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: rt $ $Date: 2007-04-03 14:18:04 $
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


// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sal.hxx"
// autogenerated file with codegen.pl

#include <math.h>
#include <stdio.h>

#include <algorithm> // STL

#include <cppunit/simpleheader.hxx>
#include "stringhelper.hxx"
#include "valueequal.hxx"

inline void printOUString( ::rtl::OUString const & _suStr )
{
    rtl::OString aString;
    
    t_print( "OUString: " );
    aString = ::rtl::OUStringToOString( _suStr, RTL_TEXTENCODING_ASCII_US );
    t_print( "'%s'\n", aString.getStr( ) );
}

namespace rtl_OUString
{
    
    class ctors_rtl_uString : public CppUnit::TestFixture
    {
        
    public:
        /// test of OUString(rtl_uString*)
        void ctors_001()
            {
                rtl::OUString *pStr = new rtl::OUString( rtl::OUString::createFromAscii("a String") );

                rtl::OUString aStrToTest(pStr->pData);
                delete pStr;
                
                // maybe here should we do something with current memory
                char* pBuffer = (char*) malloc(2 * 8);
                memset(pBuffer, 0, 2 * 8);
                free(pBuffer);

                sal_Bool bResult = aStrToTest.equals(rtl::OUString::createFromAscii("a String"));
                CPPUNIT_ASSERT_MESSAGE("String must not be empty",  bResult == sal_True);
            }
    
        // Change the following lines only, if you add, remove or rename 
        // member functions of the current class, 
        // because these macros are need by auto register mechanism.
        
        CPPUNIT_TEST_SUITE(ctors_rtl_uString);
        CPPUNIT_TEST(ctors_001);
        CPPUNIT_TEST_SUITE_END();
    };

// -----------------------------------------------------------------------------
class valueOf : public CppUnit::TestFixture
{
    void valueOf_float_test_impl(float _nValue)
        {
            rtl::OUString suValue;
            suValue = rtl::OUString::valueOf( _nValue );
            rtl::OString sValue;
            sValue <<= suValue;
            t_print(T_VERBOSE, "nFloat := %.9f  sValue := %s\n", _nValue, sValue.getStr());

            float nValueATOF = atof( sValue.getStr() );

            bool bEqualResult = is_float_equal(_nValue, nValueATOF);
            CPPUNIT_ASSERT_MESSAGE("Values are not equal.", bEqualResult == true);
        }
    
    void valueOf_float_test(float _nValue)
        {
            valueOf_float_test_impl(_nValue);

            // test also the negative part.
            float nNegativeValue = -_nValue;
            valueOf_float_test_impl(nNegativeValue);
        }

public:
    // insert your test code here.
    void valueOf_float_test_001()
    {
        // this is demonstration code
        // CPPUNIT_ASSERT_MESSAGE("a message", 1 == 1);
        float nValue = 3.0f;
        valueOf_float_test(nValue);
    }

    void valueOf_float_test_002()
    {
        float nValue = 3.5f;
        valueOf_float_test(nValue);
    }

    void valueOf_float_test_003()
    {
        float nValue = 3.0625f;
        valueOf_float_test(nValue);
    }

    void valueOf_float_test_004()
    {
        float nValue = 3.502525f;
        valueOf_float_test(nValue);
    }

    void valueOf_float_test_005()
    {
        float nValue = 3.141592f;
        valueOf_float_test(nValue);
    }

    void valueOf_float_test_006()
    {
        float nValue = 3.5025255f;
        valueOf_float_test(nValue);
    }

    void valueOf_float_test_007()
    {
        float nValue = 3.0039062f;
        valueOf_float_test(nValue);
    }

private:
    
    void valueOf_double_test_impl(double _nValue)
        {
            rtl::OUString suValue;
            suValue = rtl::OUString::valueOf( _nValue );
            rtl::OString sValue;
            sValue <<= suValue;
            t_print(T_VERBOSE, "nDouble := %.20f  sValue := %s\n", _nValue, sValue.getStr());

            double nValueATOF = atof( sValue.getStr() );
            
            bool bEqualResult = is_double_equal(_nValue, nValueATOF);
            CPPUNIT_ASSERT_MESSAGE("Values are not equal.", bEqualResult == true);
        }
    
    void valueOf_double_test(double _nValue)
        {
            valueOf_double_test_impl(_nValue);
            
            // test also the negative part.
            double nNegativeValue = -_nValue;
            valueOf_double_test_impl(nNegativeValue);
        }
public:
    
    // valueOf double
    void valueOf_double_test_001()
        {
            double nValue = 3.0;
            valueOf_double_test(nValue);
        }
    void valueOf_double_test_002()
        {
            double nValue = 3.5;
            valueOf_double_test(nValue);
        }
    void valueOf_double_test_003()
        {
            double nValue = 3.0625;
            valueOf_double_test(nValue);
        }
    void valueOf_double_test_004()
        {
            double nValue = 3.1415926535;
            valueOf_double_test(nValue);
        }
    void valueOf_double_test_005()
        {
            double nValue = 3.141592653589793;
            valueOf_double_test(nValue);
        }
    void valueOf_double_test_006()
        {
            double nValue = 3.1415926535897932;
            valueOf_double_test(nValue);
        }
    void valueOf_double_test_007()
        {
            double nValue = 3.14159265358979323;
            valueOf_double_test(nValue);
        }
    void valueOf_double_test_008()
        {
            double nValue = 3.141592653589793238462643;
            valueOf_double_test(nValue);
        }


    // Change the following lines only, if you add, remove or rename 
    // member functions of the current class, 
    // because these macros are need by auto register mechanism.

    CPPUNIT_TEST_SUITE(valueOf);
    CPPUNIT_TEST(valueOf_float_test_001);
    CPPUNIT_TEST(valueOf_float_test_002);
    CPPUNIT_TEST(valueOf_float_test_003);
    CPPUNIT_TEST(valueOf_float_test_004);
    CPPUNIT_TEST(valueOf_float_test_005);
    CPPUNIT_TEST(valueOf_float_test_006);
    CPPUNIT_TEST(valueOf_float_test_007);

    CPPUNIT_TEST(valueOf_double_test_001);
    CPPUNIT_TEST(valueOf_double_test_002);
    CPPUNIT_TEST(valueOf_double_test_003);
    CPPUNIT_TEST(valueOf_double_test_004);
    CPPUNIT_TEST(valueOf_double_test_005);
    CPPUNIT_TEST(valueOf_double_test_006);
    CPPUNIT_TEST(valueOf_double_test_007);
    CPPUNIT_TEST(valueOf_double_test_008);
    CPPUNIT_TEST_SUITE_END();
}; // class valueOf

//------------------------------------------------------------------------
// testing the method toDouble()
//------------------------------------------------------------------------
template<class T>
sal_Int16 SAL_CALL checkPrecisionSize()
{
    // sal_Int16 nSize = sizeof(T);
    volatile T nCalcValue = 1.0;

    
    // (i + 1) is the current precision 
    // numerical series
    // 1.1
    // 10.1
    // 100.1
    // ...
    // 1000...0.1

    sal_Int16 i = 0;
    for (i=0;i<50;i++)
    {
        nCalcValue *= 10;
        volatile T nValue = nCalcValue + 0.1;
        volatile T dSub = nValue - nCalcValue;
        // ----- 0.11 ---- 0.1 ---- 0.09 -----
        if (0.11 > dSub && dSub < 0.09)
        {
            // due to the fact, that the value is break down we sub 1 from the precision value
            // but to suppress this, we start at zero, precision is i+1 till here --i;
            break;
        }
    }

    sal_Int16 j= 0;
    nCalcValue = 1.0;

    // numerical series
    // 1.1
    // 1.01
    // 1.001
    // ...
    // 1.000...001
    
    for (j=0;j<50;j++)
    {
        nCalcValue /= 10;
        volatile T nValue = nCalcValue + 1.0;
        volatile T dSub = nValue - 1.0;
        // ---- 0.02 ----- 0.01 ---- 0 --- -0.99 ---- -0.98 ----
        // volatile T dSubAbsolut = fabs(dSub);
        // ---- 0.02 ----- 0.01 ---- 0 (cut)	
        if ( dSub == 0)
            break;
    }
    if (i != j)
    {
            // hmmm....
            // imho i +- 1 == j is a good value
            int n = i - j;
            if (n < 0) n = -n;
            if (n <= 1)
            {
                return std::min(i,j);
            }
            else
            {
                t_print("warning: presision differs more than 1!\n");
            }
        }
        
    return i;
}

// -----------------------------------------------------------------------------

    class testPrecision
    {
    public:
        testPrecision()
            {
                sal_Int16 nPrecision;
                nPrecision = checkPrecisionSize<float>();
                t_print("precision of float: %d sizeof()=%d \n", nPrecision, sizeof(float));
                
                nPrecision = checkPrecisionSize<double>();
                t_print("precision of double: %d sizeof()=%d \n", nPrecision, sizeof(double));
                
                nPrecision = checkPrecisionSize<long double>();
                t_print("precision of long double: %d sizeof()=%d \n", nPrecision, sizeof(long double));
                
            }
        
    };

    
// -----------------------------------------------------------------------------
// - toDouble (tests)
// -----------------------------------------------------------------------------
    class toDouble : public CppUnit::TestFixture
    {
    public:              
        void toDouble_test_impl(rtl::OString const& _sValue)
            {
                //t_print("the original str is %s\n", _sValue.getStr());
                double nValueATOF = atof( _sValue.getStr() );
        //t_print("original data is %e\n", nValueATOF);
                rtl::OUString suValue = rtl::OUString::createFromAscii( _sValue.getStr() );
                double nValueToDouble = suValue.toDouble();
                //t_print("result data is %e\n", nValueToDouble);

                bool bEqualResult = is_double_equal(nValueToDouble, nValueATOF);
                CPPUNIT_ASSERT_MESSAGE("Values are not equal.", bEqualResult == true);
            }
        
        void toDouble_test(rtl::OString const& _sValue)
            {
                toDouble_test_impl(_sValue);

                // test also the negativ part.
                rtl::OString sNegativValue("-");
                sNegativValue += _sValue;
                toDouble_test_impl(sNegativValue);
            }
        
        // insert your test code here.
        void toDouble_selftest()
            {
                t_print("Start selftest:\n");
                CPPUNIT_ASSERT (is_double_equal(1.0, 1.01) == false);
                CPPUNIT_ASSERT (is_double_equal(1.0, 1.001) == false);
                CPPUNIT_ASSERT (is_double_equal(1.0, 1.0001) == false);
                CPPUNIT_ASSERT (is_double_equal(1.0, 1.00001) == false);
                CPPUNIT_ASSERT (is_double_equal(1.0, 1.000001) == false);
                CPPUNIT_ASSERT (is_double_equal(1.0, 1.0000001) == false);
                CPPUNIT_ASSERT (is_double_equal(1.0, 1.00000001) == false);
                CPPUNIT_ASSERT (is_double_equal(1.0, 1.000000001) == false);
                CPPUNIT_ASSERT (is_double_equal(1.0, 1.0000000001) == false);
                CPPUNIT_ASSERT (is_double_equal(1.0, 1.00000000001) == false);                
                CPPUNIT_ASSERT (is_double_equal(1.0, 1.000000000001) == false);
                CPPUNIT_ASSERT (is_double_equal(1.0, 1.0000000000001) == false);
                // we check til 15 values after comma
                CPPUNIT_ASSERT (is_double_equal(1.0, 1.00000000000001) == true);
                CPPUNIT_ASSERT (is_double_equal(1.0, 1.000000000000001) == true);
                CPPUNIT_ASSERT (is_double_equal(1.0, 1.0000000000000001) == true);
                t_print("Selftest done.\n");
            }
        
        void toDouble_test_3()
            {
                rtl::OString sValue("3");
                toDouble_test(sValue);
            }
        void toDouble_test_3_5()
            {
                rtl::OString sValue("3.5");
                toDouble_test(sValue);
            }
        void toDouble_test_3_0625()
            {
                rtl::OString sValue("3.0625");
                toDouble_test(sValue);
            }
        void toDouble_test_pi()
            {
                // value from http://www.angio.net/pi/digits/50.txt
                rtl::OString sValue("3.141592653589793238462643383279502884197169399375");
                toDouble_test(sValue);
            }
        
        void toDouble_test_1()
            {
                rtl::OString sValue("1");
                toDouble_test(sValue);
            }
        void toDouble_test_10()
            {
                rtl::OString sValue("10");
                toDouble_test(sValue);
            }
        void toDouble_test_100()
            {
                rtl::OString sValue("100");
                toDouble_test(sValue);
            }
        void toDouble_test_1000()
            {
                rtl::OString sValue("1000");
                toDouble_test(sValue);
            }
        void toDouble_test_10000()
            {
                rtl::OString sValue("10000");
                toDouble_test(sValue);
            }
        void toDouble_test_1e99()
            {
                rtl::OString sValue("1e99");
                toDouble_test(sValue);
            }
        void toDouble_test_1e_n99()
            {
                rtl::OString sValue("1e-99");
                toDouble_test(sValue);
            }
        void toDouble_test_1e308()
            {
                rtl::OString sValue("1e308");
                toDouble_test(sValue);
            }
        
        // Change the following lines only, if you add, remove or rename 
        // member functions of the current class, 
        // because these macros are need by auto register mechanism.
        
        CPPUNIT_TEST_SUITE(toDouble);
        CPPUNIT_TEST(toDouble_selftest);
        
        CPPUNIT_TEST(toDouble_test_3);
        CPPUNIT_TEST(toDouble_test_3_5);
        CPPUNIT_TEST(toDouble_test_3_0625);
        CPPUNIT_TEST(toDouble_test_pi);
        CPPUNIT_TEST(toDouble_test_1);
        CPPUNIT_TEST(toDouble_test_10);
        CPPUNIT_TEST(toDouble_test_100);
        CPPUNIT_TEST(toDouble_test_1000);
        CPPUNIT_TEST(toDouble_test_10000);
        CPPUNIT_TEST(toDouble_test_1e99);
        CPPUNIT_TEST(toDouble_test_1e_n99);
        CPPUNIT_TEST(toDouble_test_1e308);
        CPPUNIT_TEST_SUITE_END();
    }; // class toDouble

// -----------------------------------------------------------------------------
// - toFloat (tests)
// -----------------------------------------------------------------------------
    class toFloat : public CppUnit::TestFixture
    {
    public:       
        void toFloat_test_impl(rtl::OString const& _sValue)
            {
                //t_print("the original str is %s\n", _sValue.getStr());
                float nValueATOF = atof( _sValue.getStr() );
        //t_print("the original str is %.10f\n", nValueATOF);
                rtl::OUString suValue = rtl::OUString::createFromAscii( _sValue.getStr() );
                float nValueToFloat = suValue.toFloat();
                //t_print("the result str is %.10f\n", nValueToFloat);

                bool bEqualResult = is_float_equal(nValueToFloat, nValueATOF);
                CPPUNIT_ASSERT_MESSAGE("Values are not equal.", bEqualResult == true);
            }
        
        void toFloat_test(rtl::OString const& _sValue)
            {
                toFloat_test_impl(_sValue);

                // test also the negativ part.
                rtl::OString sNegativValue("-");
                sNegativValue += _sValue;
                toFloat_test_impl(sNegativValue);
            }
        
        // insert your test code here.
        void toFloat_selftest()
            {
                t_print("Start selftest:\n");
                CPPUNIT_ASSERT (is_float_equal(1.0, 1.01) == false);
                CPPUNIT_ASSERT (is_float_equal(1.0, 1.001) == false);
                CPPUNIT_ASSERT (is_float_equal(1.0, 1.0001) == false);
                CPPUNIT_ASSERT (is_float_equal(1.0, 1.00001) == false);
                CPPUNIT_ASSERT (is_float_equal(1.0, 1.000002) == false);
                CPPUNIT_ASSERT (is_float_equal(1.0, 1.0000001) == true);
                CPPUNIT_ASSERT (is_float_equal(1.0, 1.00000001) == true);
                CPPUNIT_ASSERT (is_float_equal(1.0, 1.000000001) == true);
                                
                t_print("Selftest done.\n");
            }
        
        void toFloat_test_3()
            {
                rtl::OString sValue("3");
                toFloat_test(sValue);
            }
        void toFloat_test_3_5()
            {
                rtl::OString sValue("3.5");
                toFloat_test(sValue);
            }
        void toFloat_test_3_0625()
            {
                rtl::OString sValue("3.0625");
                toFloat_test(sValue);
            }
        void toFloat_test_3_0625_e()
            {
                rtl::OString sValue("3.0625e-4");
                toFloat_test(sValue);
            }
        void toFloat_test_pi()
            {
                // value from http://www.angio.net/pi/digits/50.txt
                rtl::OString sValue("3.141592653589793238462643383279502884197169399375");
                toFloat_test(sValue);
            }
        
        void toFloat_test_1()
            {
                rtl::OString sValue("1");
                toFloat_test(sValue);
            }
        void toFloat_test_10()
            {
                rtl::OString sValue("10");
                toFloat_test(sValue);
            }
        void toFloat_test_100()
            {
                rtl::OString sValue("100");
                toFloat_test(sValue);
            }
        void toFloat_test_1000()
            {
                rtl::OString sValue("1000");
                toFloat_test(sValue);
            }
        void toFloat_test_10000()
            {
                rtl::OString sValue("10000");
                toFloat_test(sValue);
            }
        void toFloat_test_mix()
            {
                rtl::OString sValue("456789321455.123456789012");
                toFloat_test(sValue);
            }
        void toFloat_test_1e99()
            {
                rtl::OString sValue("1e99");
                toFloat_test(sValue);
            }
        void toFloat_test_1e_n99()
            {
                rtl::OString sValue("1e-9");
                toFloat_test(sValue);
            }
        void toFloat_test_1e308()
            {
                rtl::OString sValue("1e308");
                toFloat_test(sValue);
            }
        
        // Change the following lines only, if you add, remove or rename 
        // member functions of the current class, 
        // because these macros are need by auto register mechanism.
        
        CPPUNIT_TEST_SUITE(toFloat);
        CPPUNIT_TEST(toFloat_selftest);
        
        CPPUNIT_TEST(toFloat_test_3);
        CPPUNIT_TEST(toFloat_test_3_5);
        CPPUNIT_TEST(toFloat_test_3_0625);
        CPPUNIT_TEST(toFloat_test_3_0625_e);
        CPPUNIT_TEST(toFloat_test_pi);
        CPPUNIT_TEST(toFloat_test_1);
        CPPUNIT_TEST(toFloat_test_10);
        CPPUNIT_TEST(toFloat_test_100);
        CPPUNIT_TEST(toFloat_test_1000);
        CPPUNIT_TEST(toFloat_test_10000);
        CPPUNIT_TEST(toFloat_test_mix);
        CPPUNIT_TEST(toFloat_test_1e99);
        CPPUNIT_TEST(toFloat_test_1e_n99);
        CPPUNIT_TEST(toFloat_test_1e308);
        CPPUNIT_TEST_SUITE_END();
    }; // class toFloat

// -----------------------------------------------------------------------------
// - lastIndexOf (tests)
// -----------------------------------------------------------------------------
class lastIndexOf : public CppUnit::TestFixture
{

public:
    void lastIndexOf_oustring(rtl::OUString const& _suStr, rtl::OUString const& _suSearchStr, sal_Int32 _nExpectedResultPos)
        {
            // Algorithm
            // search the string _suSearchStr (rtl::OUString) in the string _suStr.
            // check if the _nExpectedResultPos occurs.

            sal_Int32 nPos = _suStr.lastIndexOf(_suSearchStr);
            CPPUNIT_ASSERT_MESSAGE("expected position is wrong", nPos == _nExpectedResultPos);
        }
    
    void lastIndexOf_salunicode(rtl::OUString const& _suStr, sal_Unicode _cuSearchChar, sal_Int32 _nExpectedResultPos)
        {
            // Algorithm
            // search the unicode char _suSearchChar (sal_Unicode) in the string _suStr.
            // check if the _nExpectedResultPos occurs.

            sal_Int32 nPos = _suStr.lastIndexOf(_cuSearchChar);
            CPPUNIT_ASSERT_MESSAGE("expected position is wrong", nPos == _nExpectedResultPos);
        }
    
    void lastIndexOf_oustring_offset(rtl::OUString const& _suStr, rtl::OUString const& _suSearchStr, sal_Int32 _nExpectedResultPos, sal_Int32 _nStartOffset)
        {
            sal_Int32 nPos = _suStr.lastIndexOf(_suSearchStr, _nStartOffset);
            CPPUNIT_ASSERT_MESSAGE("expected position is wrong", nPos == _nExpectedResultPos);
        }

    void lastIndexOf_salunicode_offset(rtl::OUString const& _suStr, sal_Unicode _cuSearchChar, sal_Int32 _nExpectedResultPos, sal_Int32 _nStartOffset)
        {
            sal_Int32 nPos = _suStr.lastIndexOf(_cuSearchChar, _nStartOffset);
            CPPUNIT_ASSERT_MESSAGE("expected position is wrong", nPos == _nExpectedResultPos);
        }

    // -----------------------------------------------------------------------------

    void lastIndexOf_test_oustring_offset_001()
        {
            // search for sun, start at the end, found (pos==0)
            rtl::OUString aStr = rtl::OUString::createFromAscii("sun java system");
            rtl::OUString aSearchStr = rtl::OUString::createFromAscii("sun");
            lastIndexOf_oustring_offset(aStr, aSearchStr, 0, aStr.getLength());
        }
    
    void lastIndexOf_test_oustring_offset_002()
        {
            // search for sun, start at pos = 3, found (pos==0)
            rtl::OUString aStr = rtl::OUString::createFromAscii("sun java system");
            rtl::OUString aSearchStr = rtl::OUString::createFromAscii("sun");
            lastIndexOf_oustring_offset(aStr, aSearchStr, 0, 3);
        }
    
    void lastIndexOf_test_oustring_offset_003()
        {
            // search for sun, start at pos = 2, found (pos==-1)
            rtl::OUString aStr = rtl::OUString::createFromAscii("sun java system");
            rtl::OUString aSearchStr = rtl::OUString::createFromAscii("sun");
            lastIndexOf_oustring_offset(aStr, aSearchStr, -1, 2);
        }
    
    void lastIndexOf_test_oustring_offset_004()
        {
            // search for sun, start at the end, found (pos==0)
            rtl::OUString aStr = rtl::OUString::createFromAscii("sun java system");
            rtl::OUString aSearchStr = rtl::OUString::createFromAscii("sun");
            lastIndexOf_oustring_offset(aStr, aSearchStr, -1, -1);
        }
    
    void lastIndexOf_test_oustring_001()
        {
            // search for sun, found (pos==0)
            rtl::OUString aStr = rtl::OUString::createFromAscii("sun java system");
            rtl::OUString aSearchStr = rtl::OUString::createFromAscii("sun");
            lastIndexOf_oustring(aStr, aSearchStr, 0);
        }
    
    void lastIndexOf_test_oustring_002()
        {
            // search for sun, found (pos==4)
            rtl::OUString aStr = rtl::OUString::createFromAscii("the sun java system");
            rtl::OUString aSearchStr = rtl::OUString::createFromAscii("sun");
            lastIndexOf_oustring(aStr, aSearchStr, 4);
        }
    
    void lastIndexOf_test_oustring_003()
        {
            // search for sun, found (pos==8)
            rtl::OUString aStr = rtl::OUString::createFromAscii("the sun sun java system");
            rtl::OUString aSearchStr = rtl::OUString::createFromAscii("sun");
            lastIndexOf_oustring(aStr, aSearchStr, 8);
        }
    
    void lastIndexOf_test_oustring_004()
        {
            // search for sun, found (pos==8)
            rtl::OUString aStr = rtl::OUString::createFromAscii("the sun sun");
            rtl::OUString aSearchStr = rtl::OUString::createFromAscii("sun");
            lastIndexOf_oustring(aStr, aSearchStr, 8);
        }
    
    void lastIndexOf_test_oustring_005()
        {
            // search for sun, found (pos==4)
            rtl::OUString aStr = rtl::OUString::createFromAscii("the sun su");
            rtl::OUString aSearchStr = rtl::OUString::createFromAscii("sun");
            lastIndexOf_oustring(aStr, aSearchStr, 4);
        }
    
    void lastIndexOf_test_oustring_006()
        {
            // search for sun, found (pos==-1)
            rtl::OUString aStr = rtl::OUString::createFromAscii("the su su");
            rtl::OUString aSearchStr = rtl::OUString::createFromAscii("sun");
            lastIndexOf_oustring(aStr, aSearchStr, -1);
        }
    
    void lastIndexOf_test_oustring_007()
        {
            // search for earth, not found (-1)
            rtl::OUString aStr = rtl::OUString::createFromAscii("the su su");
            rtl::OUString aSearchStr = rtl::OUString::createFromAscii("earth");
            lastIndexOf_oustring(aStr, aSearchStr, -1);
        }
    
    void lastIndexOf_test_oustring_008()
        {
            // search for earth, not found (-1)
            rtl::OUString aStr = rtl::OUString();
            rtl::OUString aSearchStr = rtl::OUString::createFromAscii("earth");
            lastIndexOf_oustring(aStr, aSearchStr, -1);
        }
    
    void lastIndexOf_test_oustring_009()
        {
            // search for earth, not found (-1)
            rtl::OUString aStr = rtl::OUString();
            rtl::OUString aSearchStr = rtl::OUString();
            lastIndexOf_oustring(aStr, aSearchStr, -1);

        }
    
    void lastIndexOf_test_salunicode_001()
        {
            // search for 's', found (19)
            rtl::OUString aStr = rtl::OUString::createFromAscii("the sun sun java system");
            sal_Unicode suChar = L's';
            lastIndexOf_salunicode(aStr, suChar, 19);
        }

    void lastIndexOf_test_salunicode_002()
        {
            // search for 'x', not found (-1)
            rtl::OUString aStr = rtl::OUString::createFromAscii("the sun sun java system");
            sal_Unicode suChar = L'x';
            lastIndexOf_salunicode(aStr, suChar, -1);
        }

    void lastIndexOf_test_salunicode_offset_001()
        {
            // search for 's', start from pos last char, found (19)
            rtl::OUString aStr = rtl::OUString::createFromAscii("the sun sun java system");
            sal_Unicode cuChar = L's';
            lastIndexOf_salunicode_offset(aStr, cuChar, 19, aStr.getLength());
        }
    void lastIndexOf_test_salunicode_offset_002()
        {
            // search for 's', start pos is last occur from search behind, found (17)
            rtl::OUString aStr = rtl::OUString::createFromAscii("the sun sun java system");
            sal_Unicode cuChar = L's';
            lastIndexOf_salunicode_offset(aStr, cuChar, 17, 19);
        }
    void lastIndexOf_test_salunicode_offset_003()
        {
            // search for 't', start pos is 1, found (0)
            rtl::OUString aStr = rtl::OUString::createFromAscii("the sun sun java system");
            sal_Unicode cuChar = L't';
            lastIndexOf_salunicode_offset(aStr, cuChar, 0, 1);
        }
    
    // Change the following lines only, if you add, remove or rename 
    // member functions of the current class, 
    // because these macros are need by auto register mechanism.
    
    CPPUNIT_TEST_SUITE(lastIndexOf);
    CPPUNIT_TEST(lastIndexOf_test_oustring_001);
    CPPUNIT_TEST(lastIndexOf_test_oustring_002);
    CPPUNIT_TEST(lastIndexOf_test_oustring_003);
    CPPUNIT_TEST(lastIndexOf_test_oustring_004);
    CPPUNIT_TEST(lastIndexOf_test_oustring_005);
    CPPUNIT_TEST(lastIndexOf_test_oustring_006);
    CPPUNIT_TEST(lastIndexOf_test_oustring_007);
    CPPUNIT_TEST(lastIndexOf_test_oustring_008);
    CPPUNIT_TEST(lastIndexOf_test_oustring_009);

    CPPUNIT_TEST(lastIndexOf_test_oustring_offset_001);
    CPPUNIT_TEST(lastIndexOf_test_oustring_offset_002);
    CPPUNIT_TEST(lastIndexOf_test_oustring_offset_003);
    CPPUNIT_TEST(lastIndexOf_test_oustring_offset_004);

    CPPUNIT_TEST(lastIndexOf_test_salunicode_001);
    CPPUNIT_TEST(lastIndexOf_test_salunicode_002);

    CPPUNIT_TEST(lastIndexOf_test_salunicode_offset_001);
    CPPUNIT_TEST(lastIndexOf_test_salunicode_offset_002);
    CPPUNIT_TEST(lastIndexOf_test_salunicode_offset_003);

    CPPUNIT_TEST_SUITE_END();
}; // class lastIndexOf


// -----------------------------------------------------------------------------
// - getToken (tests)
// -----------------------------------------------------------------------------
class getToken : public CppUnit::TestFixture
{

public:
    void getToken_000()
        {
            rtl::OUString suTokenStr;
            
            sal_Int32 nIndex = 0;
            do
            {
                rtl::OUString suToken = suTokenStr.getToken( 0, ';', nIndex );
            }
            while ( nIndex >= 0 );
            t_print("Index %d\n", nIndex);
            // should not GPF
        }

    void getToken_001()
        {
            rtl::OUString suTokenStr = rtl::OUString::createFromAscii("a;b");
            
            sal_Int32 nIndex = 0;

            rtl::OUString suToken = suTokenStr.getToken( 0, ';', nIndex );
            CPPUNIT_ASSERT_MESSAGE("Token should be a 'a'", suToken.equals(rtl::OUString::createFromAscii("a")) == sal_True);

            /* rtl::OUString */ suToken = suTokenStr.getToken( 0, ';', nIndex );
            CPPUNIT_ASSERT_MESSAGE("Token should be a 'b'", suToken.equals(rtl::OUString::createFromAscii("b")) == sal_True);
            CPPUNIT_ASSERT_MESSAGE("index should be negative", nIndex == -1);
        }

    void getToken_002()
        {
            rtl::OUString suTokenStr = rtl::OUString::createFromAscii("a;b.c");
            
            sal_Int32 nIndex = 0;

            rtl::OUString suToken = suTokenStr.getToken( 0, ';', nIndex );
            CPPUNIT_ASSERT_MESSAGE("Token should be a 'a'", suToken.equals(rtl::OUString::createFromAscii("a")) == sal_True);

            /* rtl::OUString */ suToken = suTokenStr.getToken( 0, '.', nIndex );
            CPPUNIT_ASSERT_MESSAGE("Token should be a 'b'", suToken.equals(rtl::OUString::createFromAscii("b")) == sal_True);

            /* rtl::OUString */ suToken = suTokenStr.getToken( 0, '.', nIndex );
            CPPUNIT_ASSERT_MESSAGE("Token should be a 'c'", suToken.equals(rtl::OUString::createFromAscii("c")) == sal_True);
            CPPUNIT_ASSERT_MESSAGE("index should be negative", nIndex == -1);
        }

    void getToken_003()
        {
            rtl::OUString suTokenStr = rtl::OUString::createFromAscii("a;;b");
            
            sal_Int32 nIndex = 0;

            rtl::OUString suToken = suTokenStr.getToken( 0, ';', nIndex );
            CPPUNIT_ASSERT_MESSAGE("Token should be a 'a'", suToken.equals(rtl::OUString::createFromAscii("a")) == sal_True);

            /* rtl::OUString */ suToken = suTokenStr.getToken( 0, ';', nIndex );
            CPPUNIT_ASSERT_MESSAGE("Token should be empty", suToken.getLength() == 0);

            /* rtl::OUString */ suToken = suTokenStr.getToken( 0, ';', nIndex );
            CPPUNIT_ASSERT_MESSAGE("Token should be a 'b'", suToken.equals(rtl::OUString::createFromAscii("b")) == sal_True);
            CPPUNIT_ASSERT_MESSAGE("index should be negative", nIndex == -1);
        }

    void getToken_004()
        {
            rtl::OUString suTokenStr = rtl::OUString::createFromAscii("longer.then.ever.");
            
            sal_Int32 nIndex = 0;

            rtl::OUString suToken = suTokenStr.getToken( 0, '.', nIndex );
            CPPUNIT_ASSERT_MESSAGE("Token should be 'longer'", suToken.equals(rtl::OUString::createFromAscii("longer")) == sal_True);

            /* rtl::OUString */ suToken = suTokenStr.getToken( 0, '.', nIndex );
            CPPUNIT_ASSERT_MESSAGE("Token should be 'then'", suToken.equals(rtl::OUString::createFromAscii("then")) == sal_True);

            /* rtl::OUString */ suToken = suTokenStr.getToken( 0, '.', nIndex );
            CPPUNIT_ASSERT_MESSAGE("Token should be 'ever'", suToken.equals(rtl::OUString::createFromAscii("ever")) == sal_True);

            /* rtl::OUString */ suToken = suTokenStr.getToken( 0, '.', nIndex );
            CPPUNIT_ASSERT_MESSAGE("Token should be empty", suToken.getLength() == 0);

            CPPUNIT_ASSERT_MESSAGE("index should be negative", nIndex == -1);
        }

    void getToken_005() {
        rtl::OUString ab(RTL_CONSTASCII_USTRINGPARAM("ab"));
        sal_Int32 n = 0;
        CPPUNIT_ASSERT_MESSAGE(
            "token should be 'ab'", ab.getToken(0, '-', n) == ab);
        CPPUNIT_ASSERT_MESSAGE("n should be -1", n == -1);
        CPPUNIT_ASSERT_MESSAGE(
            "token should be empty", ab.getToken(0, '-', n).getLength() == 0);
    }

    CPPUNIT_TEST_SUITE(getToken);
    CPPUNIT_TEST(getToken_000);
    CPPUNIT_TEST(getToken_001);
    CPPUNIT_TEST(getToken_002);
    CPPUNIT_TEST(getToken_003);
    CPPUNIT_TEST(getToken_004);
    CPPUNIT_TEST(getToken_005);
    CPPUNIT_TEST_SUITE_END();
}; // class getToken

class convertToString: public CppUnit::TestFixture {
public:
    void test();

    CPPUNIT_TEST_SUITE(convertToString);
    CPPUNIT_TEST(test);
    CPPUNIT_TEST_SUITE_END();
};

void convertToString::test() {
    static sal_Unicode const utf16[] = { 0x0041, 0x00E4, 0x0061 };
    rtl::OString s;
    CPPUNIT_ASSERT(
        rtl::OUString(utf16, sizeof utf16 / sizeof utf16[0]).convertToString(
            &s, RTL_TEXTENCODING_UTF7,
            (RTL_UNICODETOTEXT_FLAGS_UNDEFINED_ERROR |
             RTL_UNICODETOTEXT_FLAGS_INVALID_ERROR)));
    CPPUNIT_ASSERT_EQUAL(
        rtl::OString(RTL_CONSTASCII_STRINGPARAM("A+AOQ-a")), s);
}

// -----------------------------------------------------------------------------
// - string construction & interning (tests)
// -----------------------------------------------------------------------------
class construction : public CppUnit::TestFixture
{
public:
    void construct()
    {
#ifdef RTL_INLINE_STRINGS
        ::rtl::OUString aFoo( RTL_CONSTASCII_USTRINGPARAM("foo") );
        CPPUNIT_ASSERT_MESSAGE("string contents", aFoo[0] == 'f');
        CPPUNIT_ASSERT_MESSAGE("string contents", aFoo[1] == 'o');
        CPPUNIT_ASSERT_MESSAGE("string contents", aFoo[2] == 'o');
        CPPUNIT_ASSERT_MESSAGE("string length", aFoo.getLength() == 3);

        ::rtl::OUString aBaa( RTL_CONSTASCII_USTRINGPARAM("this is a very long string with a lot of long things inside it and it goes on and on and on forever etc.") );
        CPPUNIT_ASSERT_MESSAGE("string length", aBaa.getLength() == 104);
        // Dig at the internals ... FIXME: should we have the bit-flag defines public ?
        CPPUNIT_ASSERT_MESSAGE("string static flags", (aBaa.pData->refCount & 1<<30) != 0);
#endif
    }

    void intern()
    {
        ::rtl::OUString aFoo( RTL_CONSTASCII_USTRINGPARAM("foo") );
        ::rtl::OUString aFooIntern = aFoo.intern();
        CPPUNIT_ASSERT_MESSAGE("string contents", aFooIntern.equalsAscii("foo"));
        CPPUNIT_ASSERT_MESSAGE("string length", aFooIntern.getLength() == 3);
        // We have to dup due to no atomic 'intern' bit-set operation
        CPPUNIT_ASSERT_MESSAGE("intern dups", aFoo.pData != aFooIntern.pData);

        // Test interning lots of things
        int i;
        static const int nSequence = 4096;
        rtl::OUString *pStrs;
        sal_uIntPtr   *pValues;

        pStrs = new rtl::OUString[nSequence];
        pValues = new sal_uIntPtr[nSequence];
        for (i = 0; i < nSequence; i++) 
        {
            pStrs[i] = rtl::OUString::valueOf( sqrt( i ) ).intern();
            pValues[i] = reinterpret_cast<sal_uIntPtr>( pStrs[i].pData );
        }
        for (i = 0; i < nSequence; i++) 
        {
            rtl::OUString aNew = rtl::OUString::valueOf( sqrt( i ) ).intern();
            CPPUNIT_ASSERT_MESSAGE("double intern failed",
                                   aNew.pData == pStrs[i].pData);
        }

        // Free strings to check for leaks
        for (i = 0; i < nSequence; i++) 
        {
            // Overwrite - hopefully this re-uses the memory
            pStrs[i] = rtl::OUString();
            pStrs[i] = rtl::OUString::valueOf( sqrt( i ) );
        }

        for (i = 0; i < nSequence; i++) 
        {
            rtl::OUString aIntern;
            sal_uIntPtr nValue;
            aIntern = rtl::OUString::valueOf( sqrt( i ) ).intern();

            nValue = reinterpret_cast<sal_uIntPtr>( aIntern.pData );
            // This may not be 100% reliable: memory may
            // have been re-used, but it's worth checking.
            CPPUNIT_ASSERT_MESSAGE("intern leaking", nValue != pValues[i]);
        }
        delete [] pValues;
        delete [] pStrs;
    }

    CPPUNIT_TEST_SUITE(construction);
    CPPUNIT_TEST(construct);
    CPPUNIT_TEST(intern);
    CPPUNIT_TEST_SUITE_END();
};

// -----------------------------------------------------------------------------
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(rtl_OUString::valueOf, "rtl_OUString");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(rtl_OUString::toDouble, "rtl_OUString");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(rtl_OUString::toFloat, "rtl_OUString");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(rtl_OUString::lastIndexOf, "rtl_OUString");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(rtl_OUString::getToken, "rtl_OUString");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(
    rtl_OUString::convertToString, "rtl_OUString");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(rtl_OUString::construction, "rtl_OUString");

} // namespace rtl_OUString


// -----------------------------------------------------------------------------

// this macro creates an empty function, which will called by the RegisterAllFunctions()
// to let the user the possibility to also register some functions by hand.
NOADDITIONAL;
