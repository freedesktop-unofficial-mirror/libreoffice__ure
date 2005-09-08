/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: unotype.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 02:10:50 $
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

#include "codemaker/unotype.hxx"

#include "osl/diagnose.h"
#include "rtl/string.hxx"
#include "sal/types.h"

#include <vector>

codemaker::UnoType::Sort codemaker::UnoType::getSort(rtl::OString const & type)
{
    return type == "void" ? SORT_VOID
        : type == "boolean" ? SORT_BOOLEAN
        : type == "byte" ? SORT_BYTE
        : type == "short" ? SORT_SHORT
        : type == "unsigned short" ? SORT_UNSIGNED_SHORT
        : type == "long" ? SORT_LONG
        : type == "unsigned long" ? SORT_UNSIGNED_LONG
        : type == "hyper" ? SORT_HYPER
        : type == "unsigned hyper" ? SORT_UNSIGNED_HYPER
        : type == "float" ? SORT_FLOAT
        : type == "double" ? SORT_DOUBLE
        : type == "char" ? SORT_CHAR
        : type == "string" ? SORT_STRING
        : type == "type" ? SORT_TYPE
        : type == "any" ? SORT_ANY
        : SORT_COMPLEX;
}

bool codemaker::UnoType::isSequenceType(rtl::OString const & type) {
    return type.getLength() > 0 && type[0] == '[';
}

rtl::OString codemaker::UnoType::decompose(
    rtl::OString const & type, sal_Int32 * rank,
    std::vector< rtl::OString > * arguments)
{
    sal_Int32 len = type.getLength();
    sal_Int32 i = 0;
    while (len - i > 1 && type[i + 1] == ']') {
        i += 2;
    }
    if (rank != 0) {
        *rank = i / 2;
    }
    sal_Int32 j = arguments == 0 ? -1 : type.indexOf('<', i);
    if (j < 0) {
        return type.copy(i);
    }
    sal_Int32 k = j;
    do {
        ++k; // skip '<' or ','
        sal_Int32 l = k;
        for (sal_Int32 level = 0; l != len; ++l) {
            char c = type[l];
            if (c == ',') {
                if (level == 0) {
                    break;
                }
            } else if (c == '<') {
                ++level;
            } else if (c == '>') {
                if (level == 0) {
                    break;
                }
                --level;
            }
        }
        arguments->push_back(type.copy(k, l - k));
        k = l;
    } while (k != len && type[k] != '>');
    OSL_ASSERT(k == len - 1 && type[k] == '>');
    return type.copy(i, j - i);
}
