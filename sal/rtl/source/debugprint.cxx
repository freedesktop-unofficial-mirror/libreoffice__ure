/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: debugprint.cxx,v $
 * $Revision: 1.6 $
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

#include <rtl/strbuf.hxx>
#include <rtl/ustring.hxx>

const sal_Char *dbg_dump(const rtl::OString &rStr)
{
    static rtl::OStringBuffer aStr;

    aStr = rtl::OStringBuffer(rStr);
    aStr.append(static_cast<char>(0));
    return aStr.getStr();
}

const sal_Char *dbg_dump(const rtl::OUString &rStr)
{
    return dbg_dump(rtl::OUStringToOString(rStr, RTL_TEXTENCODING_UTF8));
}

const sal_Char *dbg_dump(rtl_String *pStr)
{
    return dbg_dump(rtl::OString(pStr));
}

const sal_Char *dbg_dump(rtl_uString *pStr)
{
    return dbg_dump(rtl::OUString(pStr));
}

/* vi:set tabstop=4 shiftwidth=4 expandtab: */
