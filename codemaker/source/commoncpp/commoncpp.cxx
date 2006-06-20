/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: commoncpp.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-20 02:23:34 $
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
#include "sal/config.h"

#include "codemaker/commoncpp.hxx"

#include "codemaker/options.hxx"
#include "codemaker/typemanager.hxx"
#include "codemaker/unotype.hxx"

#include "osl/diagnose.h"
#include "registry/reader.hxx"
#include "registry/types.h"
#include "rtl/strbuf.hxx"
#include "rtl/string.hxx"
#include "rtl/ustring.hxx"
#include "sal/types.h"

#include <vector>

namespace codemaker { namespace cpp {

rtl::OString typeToPrefix(TypeManager const & manager, rtl::OString const & type)
{
    RTTypeClass typeclass = manager.getTypeClass(type);
    if (typeclass == RT_TYPE_INVALID ||
        typeclass == RT_TYPE_PUBLISHED)
        return rtl::OString("_");
    
    static char const * const typeclassPrefix[RT_TYPE_UNION + 1] = {
        "invalid",    /* RT_TYPE_INVALID, is here only as placeholder */
        "interface",  /* RT_TYPE_INTERFACE */
        "module",     /* RT_TYPE_MODULE */
        "struct",     /* RT_TYPE_STRUCT */
        "enum",       /* RT_TYPE_ENUM */
        "exception",  /* RT_TYPE_EXCEPTION */
        "typedef",    /* RT_TYPE_TYPEDEF */
        "service",    /* RT_TYPE_SERVICE */
        "singleton",  /* RT_TYPE_SINGLETON */
        "object",     /* RT_TYPE_OBJECT */
        "constants",  /* RT_TYPE_CONSTANTS */
        "union"       /* RT_TYPE_UNION */
    };

    return rtl::OString(typeclassPrefix[typeclass]);
}

rtl::OString scopedCppName(rtl::OString const & type, bool bNoNameSpace,
                           bool shortname)
{
    char c('/');
    sal_Int32 nPos = type.lastIndexOf( c );
    if (nPos == -1) {
        nPos = type.lastIndexOf( '.' );
        if (nPos == -1)
            return type;

        c = '.';
    }
    if (bNoNameSpace)
        return type.copy(nPos+1);

    rtl::OStringBuffer tmpBuf(type.getLength()*2);
    nPos = 0;
    do
    {
        tmpBuf.append("::");
        tmpBuf.append(type.getToken(0, c, nPos));
    } while( nPos != -1 );

    if (shortname) {
        rtl::OString s(tmpBuf.makeStringAndClear());
        if (s.indexOf("::com::sun::star") == 0)
            return s.replaceAt(0, 16, "css");
        else
            return s;
    }
    
    return tmpBuf.makeStringAndClear();
}	


rtl::OString translateUnoToCppType(
    codemaker::UnoType::Sort sort, RTTypeClass typeClass,
    rtl::OString const & nucleus, bool shortname)
{
    rtl::OStringBuffer buf;
    if (sort == codemaker::UnoType::SORT_COMPLEX) {
        if (typeClass == RT_TYPE_INTERFACE
            && nucleus == rtl::OString("com/sun/star/uno/XInterface"))
        {
            buf.append(RTL_CONSTASCII_STRINGPARAM("::com::sun::star::uno::XInterface"));
        } else {
            //TODO: check that nucleus is a valid (UTF-8) identifier
            buf.append(nucleus);
        }
    } else {
        static char const * const cppTypes[codemaker::UnoType::SORT_ANY + 1] = {
            "void", "::sal_Bool", "::sal_Int8", "::sal_Int16", "::sal_uInt16",
            "::sal_Int32", "::sal_uInt32", "::sal_Int64", "::sal_uInt64",
            "float", "double", "::sal_Unicode", "::rtl::OUString",
            "::com::sun::star::uno::Type", "::com::sun::star::uno::Any" };
        buf.append(cppTypes[sort]);
    }

    if (shortname) {
        rtl::OString s(buf.makeStringAndClear());
        if (s.indexOf("::com::sun::star") == 0)
            return s.replaceAt(0, 16, "css");
        else
            return s;
    }

    return buf.makeStringAndClear();
}

rtl::OString translateUnoToCppIdentifier(
    rtl::OString const & unoIdentifier, rtl::OString const & prefix,
    IdentifierTranslationMode transmode, rtl::OString const * forbidden)
{
    if (// Keywords:
        unoIdentifier == "asm"
        || unoIdentifier == "auto"
        || unoIdentifier == "bool"
        || unoIdentifier == "break"
        || unoIdentifier == "case"
        || unoIdentifier == "catch"
        || unoIdentifier == "char"
        || unoIdentifier == "class"
        || unoIdentifier == "const"
        /* unoIdentifier == "const_cast" */
        || unoIdentifier == "continue"
        || unoIdentifier == "default"
        || unoIdentifier == "delete"
        || unoIdentifier == "do"
        || unoIdentifier == "double"
        /* unoIdentifier == "dynamic_cast" */
        || unoIdentifier == "else"
        || unoIdentifier == "enum"
        || unoIdentifier == "explicit"
        || unoIdentifier == "export"
        || unoIdentifier == "extern"
        || unoIdentifier == "false"
        || unoIdentifier == "float"
        || unoIdentifier == "for"
        || unoIdentifier == "friend"
        || unoIdentifier == "goto"
        || unoIdentifier == "if"
        || unoIdentifier == "inline"
        || unoIdentifier == "int"
        || unoIdentifier == "long"
        || unoIdentifier == "mutable"
        || unoIdentifier == "namespace"
        || unoIdentifier == "new"
        || unoIdentifier == "operator"
        || unoIdentifier == "private"
        || unoIdentifier == "protected"
        || unoIdentifier == "public"
        || unoIdentifier == "register"
        /* unoIdentifier == "reinterpret_cast" */
        || unoIdentifier == "return"
        || unoIdentifier == "short"
        || unoIdentifier == "signed"
        || unoIdentifier == "sizeof"
        || unoIdentifier == "static"
        /* unoIdentifier == "static_cast" */
        || unoIdentifier == "struct"
        || unoIdentifier == "switch"
        || unoIdentifier == "template"
        || unoIdentifier == "this"
        || unoIdentifier == "throw"
        || unoIdentifier == "true"
        || unoIdentifier == "try"
        || unoIdentifier == "typedef"
        || unoIdentifier == "typeid"
        || unoIdentifier == "typename"
        || unoIdentifier == "union"
        || unoIdentifier == "unsigned"
        || unoIdentifier == "using"
        || unoIdentifier == "virtual"
        || unoIdentifier == "void"
        || unoIdentifier == "volatile"
        /* unoIdentifier == "wchar_t" */
        || unoIdentifier == "while"
        // Alternative representations:
        || unoIdentifier == "and"
        /* unoIdentifier == "and_eq" */
        || unoIdentifier == "bitand"
        || unoIdentifier == "bitor"
        || unoIdentifier == "compl"
        || unoIdentifier == "not"
        /* unoIdentifier == "not_eq" */
        || unoIdentifier == "or"
        /* unoIdentifier == "or_eq" */
        || unoIdentifier == "xor"
        /* unoIdentifier == "xor_eq" */
        // Standard macros:
        || (transmode != ITM_KEYWORDSONLY
            && (unoIdentifier == "BUFSIZ"
                || unoIdentifier == "CLOCKS_PER_SEC"
                || unoIdentifier == "EDOM"
                || unoIdentifier == "EOF"
                || unoIdentifier == "ERANGE"
                || unoIdentifier == "EXIT_FAILURE"
                || unoIdentifier == "EXIT_SUCCESS"
                || unoIdentifier == "FILENAME_MAX"
                || unoIdentifier == "FOPEN_MAX"
                || unoIdentifier == "HUGE_VAL"
                || unoIdentifier == "LC_ALL"
                || unoIdentifier == "LC_COLLATE"
                || unoIdentifier == "LC_CTYPE"
                || unoIdentifier == "LC_MONETARY"
                || unoIdentifier == "LC_NUMERIC"
                || unoIdentifier == "LC_TIME"
                || unoIdentifier == "L_tmpnam"
                || unoIdentifier == "MB_CUR_MAX"
                || unoIdentifier == "NULL"
                || unoIdentifier == "RAND_MAX"
                || unoIdentifier == "SEEK_CUR"
                || unoIdentifier == "SEEK_END"
                || unoIdentifier == "SEEK_SET"
                || unoIdentifier == "SIGABRT"
                || unoIdentifier == "SIGFPE"
                || unoIdentifier == "SIGILL"
                || unoIdentifier == "SIGINT"
                || unoIdentifier == "SIGSEGV"
                || unoIdentifier == "SIGTERM"
                || unoIdentifier == "SIG_DFL"
                || unoIdentifier == "SIG_ERR"
                || unoIdentifier == "SIG_IGN"
                || unoIdentifier == "TMP_MAX"
                || unoIdentifier == "WCHAR_MAX"
                || unoIdentifier == "WCHAR_MIN"
                || unoIdentifier == "WEOF"
                /* unoIdentifier == "_IOFBF" */
                /* unoIdentifier == "_IOLBF" */
                /* unoIdentifier == "_IONBF" */
                || unoIdentifier == "assert"
                || unoIdentifier == "errno"
                || unoIdentifier == "offsetof"
                || unoIdentifier == "setjmp"
                || unoIdentifier == "stderr"
                || unoIdentifier == "stdin"
                || unoIdentifier == "stdout"
                /* unoIdentifier == "va_arg" */
                /* unoIdentifier == "va_end" */
                /* unoIdentifier == "va_start" */
                // Standard values:
                || unoIdentifier == "CHAR_BIT"
                || unoIdentifier == "CHAR_MAX"
                || unoIdentifier == "CHAR_MIN"
                || unoIdentifier == "DBL_DIG"
                || unoIdentifier == "DBL_EPSILON"
                || unoIdentifier == "DBL_MANT_DIG"
                || unoIdentifier == "DBL_MAX"
                || unoIdentifier == "DBL_MAX_10_EXP"
                || unoIdentifier == "DBL_MAX_EXP"
                || unoIdentifier == "DBL_MIN"
                || unoIdentifier == "DBL_MIN_10_EXP"
                || unoIdentifier == "DBL_MIN_EXP"
                || unoIdentifier == "FLT_DIG"
                || unoIdentifier == "FLT_EPSILON"
                || unoIdentifier == "FLT_MANT_DIG"
                || unoIdentifier == "FLT_MAX"
                || unoIdentifier == "FLT_MAX_10_EXP"
                || unoIdentifier == "FLT_MAX_EXP"
                || unoIdentifier == "FLT_MIN"
                || unoIdentifier == "FLT_MIN_10_EXP"
                || unoIdentifier == "FLT_MIN_EXP"
                || unoIdentifier == "FLT_RADIX"
                || unoIdentifier == "FLT_ROUNDS"
                || unoIdentifier == "INT_MAX"
                || unoIdentifier == "INT_MIN"
                || unoIdentifier == "LDBL_DIG"
                || unoIdentifier == "LDBL_EPSILON"
                || unoIdentifier == "LDBL_MANT_DIG"
                || unoIdentifier == "LDBL_MAX"
                || unoIdentifier == "LDBL_MAX_10_EXP"
                || unoIdentifier == "LDBL_MAX_EXP"
                || unoIdentifier == "LDBL_MIN"
                || unoIdentifier == "LDBL_MIN_10_EXP"
                || unoIdentifier == "LDBL_MIN_EXP"
                || unoIdentifier == "LONG_MAX"
                || unoIdentifier == "LONG_MIN"
                || unoIdentifier == "MB_LEN_MAX"
                || unoIdentifier == "SCHAR_MAX"
                || unoIdentifier == "SCHAR_MIN"
                || unoIdentifier == "SHRT_MAX"
                || unoIdentifier == "SHRT_MIN"
                || unoIdentifier == "UCHAR_MAX"
                || unoIdentifier == "UINT_MAX"
                || unoIdentifier == "ULONG_MAX"
                || unoIdentifier == "USHRT_MAX"))
            || (transmode == ITM_GLOBAL
                && (// Standard types:
                    /* unoIdentifier == "clock_t" */
                    /* unoIdentifier == "div_t" */
                    unoIdentifier == "FILE"
                    /* unoIdentifier == "fpos_t" */
                    /* unoIdentifier == "jmp_buf" */
                    || unoIdentifier == "lconv"
                    /* unoIdentifier == "ldiv_t" */
                    /* unoIdentifier == "mbstate_t" */
                    /* unoIdentifier == "ptrdiff_t" */
                    /* unoIdentifier == "sig_atomic_t" */
                    /* unoIdentifier == "size_t" */
                    /* unoIdentifier == "time_t" */
                    || unoIdentifier == "tm"
                    /* unoIdentifier == "va_list" */
                    /* unoIdentifier == "wctrans_t" */
                    /* unoIdentifier == "wctype_t" */
                    /* unoIdentifier == "wint_t" */
                    // Standard namespaces:
                    || unoIdentifier == "std"))
            // Others:
            || unoIdentifier == "NDEBUG"
            || forbidden != 0 && unoIdentifier == *forbidden)
    {
        rtl::OStringBuffer buf(prefix);
        buf.append('_');
        buf.append(unoIdentifier);
        return buf.makeStringAndClear();
    } else {
        return unoIdentifier;
    }
}

} }
