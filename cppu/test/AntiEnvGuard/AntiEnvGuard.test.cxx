/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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

#include "sal/main.h"

#include "cppu/EnvGuards.hxx"
#include "uno/environment.hxx"

#include <iostream>


using namespace com::sun::star;

static rtl::OUString    s_message;


static void s_test__ctor(void)
{
    s_message += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("\ts_test__ctor"));

    rtl::OUString ref(RTL_CONSTASCII_USTRINGPARAM("uno"));

    rtl::OUString current_EnvDcp;
    {
        cppu::EnvGuard envGuard(uno::Environment(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("uno:unsafe")), NULL));

        {
            cppu::AntiEnvGuard antiGuard;

            current_EnvDcp = uno::Environment::getCurrent(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(UNO_LB_UNO))).getTypeName();
        }
    }

    if (current_EnvDcp == ref)
        s_message += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("\n"));

    else
    {
        s_message += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" - FAILURE\n"));
        s_message += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("\t        got: \""));
        s_message += current_EnvDcp;
        s_message += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("\"\n"));
        s_message += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("\t   expected: \""));
        s_message += ref;
        s_message += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("\"\n"));
    }
}

static void s_test__dtor(void)
{
    s_message += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("\ts_test__dtor"));

    rtl::OUString ref(RTL_CONSTASCII_USTRINGPARAM("uno:unsafe"));

    rtl::OUString current_EnvDcp;
    {
        cppu::EnvGuard envGuard(uno::Environment(ref, NULL));

        {
            cppu::AntiEnvGuard antiGuard;
        }
        current_EnvDcp = uno::Environment::getCurrent(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(UNO_LB_UNO))).getTypeName();
    }


    if (current_EnvDcp == ref)
        s_message += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("\n"));

    else
    {
        s_message += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" - FAILURE\n"));
        s_message += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("\t        got: \""));
        s_message += current_EnvDcp;
        s_message += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("\"\n"));
        s_message += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("\t   expected: \""));
        s_message += ref;
        s_message += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("\"\n"));
    }
}



SAL_IMPLEMENT_MAIN_WITH_ARGS(/*argc*/, argv)
{
    s_test__ctor();
    s_test__dtor();

    int ret;
    if (s_message.indexOf(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("FAILURE"))) == -1)
    {
        s_message += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("TESTS PASSED\n"));
        ret = 0;
    }
    else 
    {
        s_message += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("TESTS _NOT_ PASSED\n"));
        ret = -1;
    }

    std::cerr 
        << argv[0] 
        << std::endl 
        << rtl::OUStringToOString(s_message, RTL_TEXTENCODING_ASCII_US).getStr() 
        << std::endl;

    return ret;
}
