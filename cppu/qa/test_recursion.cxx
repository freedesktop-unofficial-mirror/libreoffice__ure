/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: test_recursion.cxx,v $
 * $Revision: 1.1.2.1 $
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

#include "precompiled_cppu.hxx"
#include "sal/config.h"

#include "cppunit/simpleheader.hxx"
#include "sal/types.h"

#include "Rec.hpp"

namespace
{

class Test: public CppUnit::TestFixture {

public:
    void testRecursion();

    CPPUNIT_TEST_SUITE(Test);
    CPPUNIT_TEST(testRecursion);
    CPPUNIT_TEST_SUITE_END();
};

void Test::testRecursion() {
    CPPUNIT_ASSERT_EQUAL(static_cast< sal_Int32 >(0), Rec().x.getLength());
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Test, "tests");

}

NOADDITIONAL;
