/*************************************************************************
 *
 *  $RCSfile: test_oustringbuffer_utf32.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: kz $ $Date: 2004-02-26 13:37:11 $
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

#include "cppunit/simpleheader.hxx"
#include "rtl/ustrbuf.hxx"
#include "rtl/ustring.h"
#include "rtl/ustring.hxx"

namespace test { namespace oustringbuffer {

class Utf32: public CppUnit::TestFixture {
private:
    void appendUtf32();

    void insertUtf32();

    CPPUNIT_TEST_SUITE(Utf32);
    CPPUNIT_TEST(appendUtf32);
    CPPUNIT_TEST(insertUtf32);
    CPPUNIT_TEST_SUITE_END();
};

} }

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(test::oustringbuffer::Utf32, "alltest");

namespace {

void appendString(rtl::OUStringBuffer & buffer, rtl::OUString const & string) {
    buffer.append(static_cast< sal_Unicode >('"'));
    for (int i = 0; i < string.getLength(); ++i) {
        buffer.appendAscii(RTL_CONSTASCII_STRINGPARAM("\\u"));
        sal_Unicode c = string[i];
        if (c < 0x1000) {
            buffer.append(static_cast< sal_Unicode >('0'));
            if (c < 0x100) {
                buffer.append(static_cast< sal_Unicode >('0'));
                if (c < 0x10) {
                    buffer.append(static_cast< sal_Unicode >('0'));
                }
            }
        }
        buffer.append(
            static_cast< sal_Int32 >(c), static_cast< sal_Int16 >(16));
    }
    buffer.append(static_cast< sal_Unicode >('"'));
}

void createMessage(
    rtl::OUStringBuffer & message, rtl::OUString const & string1,
    rtl::OUString const & string2)
{
    message.setLength(0);
    appendString(message, string1);
    message.appendAscii(RTL_CONSTASCII_STRINGPARAM(" vs. "));
    appendString(message, string2);
}

}

void test::oustringbuffer::Utf32::appendUtf32() {
    int const str1Len = 3;
    sal_Unicode const str1[str1Len] = { 'a', 'b', 'c' };
    int const str2Len = 4;
    sal_Unicode const str2[str2Len] = { 'a', 'b', 'c', 'd' };
    int const str3Len = 6;
    sal_Unicode const str3[str3Len] = { 'a', 'b', 'c', 'd', 0xD800, 0xDC00 };
    rtl::OUStringBuffer message;
    rtl::OUStringBuffer buf1(rtl::OUString(str1, str1Len));
    buf1.appendUtf32('d');
    rtl::OUString res1(buf1.makeStringAndClear());
    createMessage(message, res1, rtl::OUString(str2, str2Len));
    CPPUNIT_ASSERT_MESSAGE(
        message.getStr(), res1 == rtl::OUString(str2, str2Len));
    rtl::OUStringBuffer buf2(rtl::OUString(str2, str2Len));
    buf2.appendUtf32(0x10000);
    rtl::OUString res2(buf2.makeStringAndClear());
    createMessage(message, res2, rtl::OUString(str3, str3Len));
    CPPUNIT_ASSERT_MESSAGE(
        message.getStr(), res2 == rtl::OUString(str3, str3Len));
}

void test::oustringbuffer::Utf32::insertUtf32() {
    int const str1Len = 3;
    sal_Unicode const str1[str1Len] = { 'a', 'b', 'c' };
    int const str2Len = 4;
    sal_Unicode const str2[str2Len] = { 'a', 'b', 'd', 'c' };
    int const str3Len = 6;
    sal_Unicode const str3[str3Len] = { 'a', 'b', 0xDBFF, 0xDFFF, 'd', 'c' };
    rtl::OUStringBuffer message;
    rtl::OUStringBuffer buf1(rtl::OUString(str1, str1Len));
    buf1.insertUtf32(2, 'd');
    rtl::OUString res1(buf1.makeStringAndClear());
    createMessage(message, res1, rtl::OUString(str2, str2Len));
    CPPUNIT_ASSERT_MESSAGE(
        message.getStr(), res1 == rtl::OUString(str2, str2Len));
    rtl::OUStringBuffer buf2(rtl::OUString(str2, str2Len));
    buf2.insertUtf32(2, 0x10FFFF);
    rtl::OUString res2(buf2.makeStringAndClear());
    createMessage(message, res2, rtl::OUString(str3, str3Len));
    CPPUNIT_ASSERT_MESSAGE(
        message.getStr(), res2 == rtl::OUString(str3, str3Len));
}
