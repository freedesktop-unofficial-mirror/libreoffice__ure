/*************************************************************************
 *
 *  $RCSfile: jvmargs.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: cp $ $Date: 2002-01-20 20:11:04 $
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


#ifndef __JVM_HXX
#define __JVM_HXX

#include <cstdarg>
#include <vector>


#include <rtl/ustring>

#include "jni.h"


typedef    jint (JNICALL *JNIvfprintf)(FILE *fp, const char *format, va_list args);
typedef    void (JNICALL *JNIexit)(jint code);
typedef    void (JNICALL *JNIabort)(void);

extern "C" {
    typedef jint JNICALL JNI_InitArgs_Type(void *);
    typedef jint JNICALL JNI_CreateVM_Type(JavaVM **, JNIEnv **, void *);

}

namespace stoc_javavm {

    class JVM {
        ::std::vector<rtl::OUString> _props;
        ::rtl::OUString _runtimeLib;
        ::rtl::OUString _systemClasspath;
        ::rtl::OUString _userClasspath;
        sal_Bool _enabled;

        sal_Bool _is_debugPort;
        jint     _debugPort;

        sal_Bool _is_disableAsyncGC;
        jint      _disableAsyncGC;

        sal_Bool _is_enableClassGC;
        jint     _enableClassGC;

        sal_Bool _is_enableVerboseGC;
        jint     _enableVerboseGC;

        sal_Bool _is_checkSource;
        jint     _checkSource;

        sal_Bool _is_nativeStackSize;
        jint     _nativeStackSize;

        sal_Bool _is_javaStackSize;
        jint     _javaStackSize;

        sal_Bool _is_minHeapSize;
        jint     _minHeapSize;
    
        sal_Bool _is_maxHeapSize;
        jint     _maxHeapSize;

        sal_Bool _is_verifyMode;
        jint     _verifyMode;

        sal_Bool _is_print;
        JNIvfprintf _print;

        sal_Bool _is_exit;
        JNIexit  _exit;

        sal_Bool _is_abort;
        JNIabort _abort;

        ::rtl::OUString _java_home;
        ::rtl::OUString _vmtype;

    public:
        JVM() throw();

        void pushProp(const ::rtl::OUString & uString) throw();

        void setEnabled(sal_Bool sbFlag) throw();
        void setDisableAsyncGC(jint jiFlag) throw();
        void setEnableClassGC(jint jiFlag) throw();
        void setEnableVerboseGC(jint jiFlag) throw();
        void setCheckSource(jint jiFlag) throw();
        void setNativeStackSize(jint jiSize) throw();
        void setJavaStackSize(jint jiSize) throw();
        void setVerifyMode(const ::rtl::OUString & mode) throw();
        void setMinHeapSize(jint jiSize) throw();
        void setMaxHeapSize(jint jiSize) throw();
        void setDebugPort(jint jiDebugPort) throw();
        void setSystemClasspath(const ::rtl::OUString & str) throw();
        void setUserClasspath(const ::rtl::OUString & str) throw();
        void setPrint(JNIvfprintf vfprintf) throw();
        void setExit(JNIexit exit) throw();
        void setAbort(JNIabort abort) throw();
        void setRuntimeLib(const ::rtl::OUString & libName) throw();

        void setArgs(JDK1_1InitArgs * pargs) const throw();

        const ::rtl::OUString & getRuntimeLib() const throw();
        sal_Bool isEnabled() const throw();
        const ::std::vector< ::rtl::OUString> & getProperties() const;

        const ::rtl::OUString & getJavaHome() const throw();
        const ::rtl::OUString & getVMType() const throw();
    };
}

#endif
