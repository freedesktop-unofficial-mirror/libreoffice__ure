/*************************************************************************
 *
 *  $RCSfile: jni_bridge.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: vg $ $Date: 2003-04-15 16:26:59 $
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

#include "jni_bridge.h"

#include "rtl/unload.h"
#include "rtl/strbuf.hxx"
#include "uno/lbnames.h"


using namespace ::std;
using namespace ::rtl;
using namespace ::osl;
using namespace ::jni_uno;

namespace
{
extern "C"
{

//--------------------------------------------------------------------------------------------------
void SAL_CALL Mapping_acquire( uno_Mapping * mapping )
    SAL_THROW_EXTERN_C()
{
    Mapping const * that = static_cast< Mapping const * >( mapping );
    that->m_bridge->acquire();
}
//--------------------------------------------------------------------------------------------------
void SAL_CALL Mapping_release( uno_Mapping * mapping )
    SAL_THROW_EXTERN_C()
{
    Mapping const * that = static_cast< Mapping const * >( mapping );
    that->m_bridge->release();
}
//--------------------------------------------------------------------------------------------------
void SAL_CALL Mapping_map_to_uno(
    uno_Mapping * mapping, void ** ppOut,
    void * pIn, typelib_InterfaceTypeDescription * td )
    SAL_THROW_EXTERN_C()
{
    uno_Interface ** ppUnoI = (uno_Interface **)ppOut;
    jobject javaI = (jobject)pIn;

    OSL_ASSERT( sizeof (void *) == sizeof (jobject) );
    OSL_ENSURE( ppUnoI && td, "### null ptr!" );

    if (0 == javaI)
    {
        if (0 != *ppUnoI)
        {
            uno_Interface * p = *(uno_Interface **)ppUnoI;
            (*p->release)( p );
            *ppUnoI = 0;
        }
    }
    else
    {
        try
        {
            Bridge const * bridge = static_cast< Mapping const * >( mapping )->m_bridge;
            JNI_guarded_context jni(
                bridge->m_jni_info,
                reinterpret_cast< ::jvmaccess::VirtualMachine * >( bridge->m_java_env->pContext ) );

            JNI_interface_type_info const * info =
                static_cast< JNI_interface_type_info const * >(
                    bridge->m_jni_info->get_type_info( jni, (typelib_TypeDescription *)td ) );
            uno_Interface * pUnoI = bridge->map_to_uno( jni, javaI, info );
            if (0 != *ppUnoI)
            {
                uno_Interface * p = *(uno_Interface **)ppUnoI;
                (*p->release)( p );
            }
            *ppUnoI = pUnoI;
        }
        catch (BridgeRuntimeError & err)
        {
#if OSL_DEBUG_LEVEL > 0
            OString cstr_msg(
                OUStringToOString(
                    OUSTR("[jni_uno bridge error] ") + err.m_message, RTL_TEXTENCODING_ASCII_US ) );
            OSL_ENSURE( 0, cstr_msg.getStr() );
#endif
        }
        catch (::jvmaccess::VirtualMachine::AttachGuard::CreationException &)
        {
            OSL_ENSURE( 0, "[jni_uno bridge error] attaching current thread to java failed!" );
        }
    }
}
//--------------------------------------------------------------------------------------------------
void SAL_CALL Mapping_map_to_java(
    uno_Mapping * mapping, void ** ppOut,
    void * pIn, typelib_InterfaceTypeDescription * td )
    SAL_THROW_EXTERN_C()
{
    jobject * ppJavaI = (jobject *)ppOut;
    uno_Interface * pUnoI = (uno_Interface *)pIn;

    OSL_ASSERT( sizeof (void *) == sizeof (jobject) );
    OSL_ENSURE( ppJavaI && td, "### null ptr!" );

    try
    {
        if (0 == pUnoI)
        {
            if (0 != *ppJavaI)
            {
                Bridge const * bridge = static_cast< Mapping const * >( mapping )->m_bridge;
                JNI_guarded_context jni(
                    bridge->m_jni_info,
                    reinterpret_cast< ::jvmaccess::VirtualMachine * >(
                        bridge->m_java_env->pContext ) );
                jni->DeleteGlobalRef( *ppJavaI );
                *ppJavaI = 0;
            }
        }
        else
        {
            Bridge const * bridge = static_cast< Mapping const * >( mapping )->m_bridge;
            JNI_guarded_context jni(
                bridge->m_jni_info,
                reinterpret_cast< ::jvmaccess::VirtualMachine * >( bridge->m_java_env->pContext ) );

            JNI_interface_type_info const * info =
                static_cast< JNI_interface_type_info const * >(
                    bridge->m_jni_info->get_type_info( jni, (typelib_TypeDescription *)td ) );
            jobject jlocal = bridge->map_to_java( jni, pUnoI, info );
            if (0 != *ppJavaI)
                jni->DeleteGlobalRef( *ppJavaI );
            *ppJavaI = jni->NewGlobalRef( jlocal );
            jni->DeleteLocalRef( jlocal );
        }
    }
    catch (BridgeRuntimeError & err)
    {
#if OSL_DEBUG_LEVEL > 0
        OString cstr_msg(
            OUStringToOString(
                OUSTR("[jni_uno bridge error] ") + err.m_message, RTL_TEXTENCODING_ASCII_US ) );
        OSL_ENSURE( 0, cstr_msg.getStr() );
#endif
    }
    catch (::jvmaccess::VirtualMachine::AttachGuard::CreationException &)
    {
        OSL_ENSURE( 0, "[jni_uno bridge error] attaching current thread to java failed!" );
    }
}
//__________________________________________________________________________________________________
void SAL_CALL Bridge_free( uno_Mapping * mapping )
    SAL_THROW_EXTERN_C()
{
    Mapping * that = static_cast< Mapping * >( mapping );
    delete that->m_bridge;
}

}

//==================================================================================================
rtl_StandardModuleCount g_moduleCount = MODULE_COUNT_INIT;

}

namespace jni_uno
{

//__________________________________________________________________________________________________
void Bridge::acquire() const SAL_THROW( () )
{
    if (1 == osl_incrementInterlockedCount( &m_ref ))
    {
        if (m_registered_java2uno)
        {
            uno_Mapping * mapping = const_cast< Mapping * >( &m_java2uno );
            uno_registerMapping(
                &mapping, Bridge_free, m_java_env, (uno_Environment *)m_uno_env, 0 );
        }
        else
        {
            uno_Mapping * mapping = const_cast< Mapping * >( &m_uno2java );
            uno_registerMapping(
                &mapping, Bridge_free, (uno_Environment *)m_uno_env, m_java_env, 0 );
        }
    }
}
//__________________________________________________________________________________________________
void Bridge::release() const SAL_THROW( () )
{
    if (! osl_decrementInterlockedCount( &m_ref ))
    {
        uno_revokeMapping(
            m_registered_java2uno
            ? const_cast< Mapping * >( &m_java2uno )
            : const_cast< Mapping * >( &m_uno2java ) );
    }
}
//__________________________________________________________________________________________________
Bridge::Bridge(
    uno_Environment * java_env, uno_ExtEnvironment * uno_env,
    bool registered_java2uno )
    : m_ref( 1 ),
      m_uno_env( uno_env ),
      m_java_env( java_env ),
      m_registered_java2uno( registered_java2uno )
{
    // bootstrapping bridge jni_info
    ::jvmaccess::VirtualMachine::AttachGuard jni(
        reinterpret_cast< ::jvmaccess::VirtualMachine * >( m_java_env->pContext ) );
    m_jni_info = JNI_info::get_jni_info( jni.getEnvironment() );

    OSL_ASSERT( 0 != m_java_env && 0 != m_uno_env );
    (*((uno_Environment *)m_uno_env)->acquire)( (uno_Environment *)m_uno_env );
    (*m_java_env->acquire)( m_java_env );

    // java2uno
    m_java2uno.acquire = Mapping_acquire;
    m_java2uno.release = Mapping_release;
    m_java2uno.mapInterface = Mapping_map_to_uno;
    m_java2uno.m_bridge = this;
    // uno2java
    m_uno2java.acquire = Mapping_acquire;
    m_uno2java.release = Mapping_release;
    m_uno2java.mapInterface = Mapping_map_to_java;
    m_uno2java.m_bridge = this;

    (*g_moduleCount.modCnt.acquire)( &g_moduleCount.modCnt );
}
//__________________________________________________________________________________________________
Bridge::~Bridge() SAL_THROW( () )
{
    (*m_java_env->release)( m_java_env );
    (*((uno_Environment *)m_uno_env)->release)( (uno_Environment *)m_uno_env );

    (*g_moduleCount.modCnt.release)( &g_moduleCount.modCnt );
}

//##################################################################################################

//__________________________________________________________________________________________________
void JNI_context::java_exc_occured() const
{
    // !don't rely on JNI_info!

    JLocalAutoRef jo_exc( *this, m_env->ExceptionOccurred() );
    m_env->ExceptionClear();
    OSL_ASSERT( jo_exc.is() );
    if (! jo_exc.is())
    {
        throw BridgeRuntimeError(
            OUSTR("java exception occured, but not available!?") + get_stack_trace() );
    }

    // call toString(); don't rely on m_jni_info
    jclass jo_class = m_env->FindClass( "java/lang/Object" );
    if (JNI_FALSE != m_env->ExceptionCheck())
    {
        m_env->ExceptionClear();
        throw BridgeRuntimeError(
            OUSTR("cannot get class java.lang.Object!") + get_stack_trace() );
    }
    JLocalAutoRef jo_Object( *this, jo_class );
    // method Object.toString()
    jmethodID method_Object_toString = m_env->GetMethodID(
        (jclass)jo_Object.get(), "toString", "()Ljava/lang/String;" );
    if (JNI_FALSE != m_env->ExceptionCheck())
    {
        m_env->ExceptionClear();
        throw BridgeRuntimeError(
            OUSTR("cannot get method id of java.lang.Object.toString()!") + get_stack_trace() );
    }
    OSL_ASSERT( 0 != method_Object_toString );

    JLocalAutoRef jo_descr(
        *this, m_env->CallObjectMethodA( jo_exc.get(), method_Object_toString, 0 ) );
    if (m_env->ExceptionCheck()) // no chance at all
    {
        m_env->ExceptionClear();
        throw BridgeRuntimeError(
            OUSTR("error examining java exception object!") + get_stack_trace() );
    }

    jsize len = m_env->GetStringLength( (jstring)jo_descr.get() );
    auto_ptr< rtl_mem > ustr_mem(
        rtl_mem::allocate( sizeof (rtl_uString) + (len * sizeof (sal_Unicode)) ) );
    rtl_uString * ustr = (rtl_uString *)ustr_mem.get();
    m_env->GetStringRegion( (jstring)jo_descr.get(), 0, len, ustr->buffer );
    if (m_env->ExceptionCheck())
    {
        m_env->ExceptionClear();
        throw BridgeRuntimeError(
            OUSTR("invalid java string object!") + get_stack_trace() );
    }
    ustr->refCount = 1;
    ustr->length = len;
    ustr->buffer[ len ] = '\0';
    OUString message( (rtl_uString *)ustr_mem.release(), SAL_NO_ACQUIRE );

    throw BridgeRuntimeError( message + get_stack_trace( jo_exc.get() ) );
}
//__________________________________________________________________________________________________
OUString JNI_context::get_stack_trace( jobject jo_exc ) const
{
    JLocalAutoRef jo_JNI_proxy(
        *this, m_env->FindClass( "com/sun/star/bridges/jni_uno/JNI_proxy" ) );
    if (assert_no_exception())
    {
        // static method JNI_proxy.get_stack_trace()
        jmethodID method = m_env->GetStaticMethodID(
            (jclass)jo_JNI_proxy.get(), "get_stack_trace",
            "(Ljava/lang/Throwable;)Ljava/lang/String;" );
        if (assert_no_exception() && (0 != method))
        {
            jvalue arg;
            arg.l = jo_exc;
            JLocalAutoRef jo_stack_trace(
                *this, m_env->CallStaticObjectMethodA( (jclass)jo_JNI_proxy.get(), method, &arg ) );
            if (assert_no_exception())
            {
                jsize len = m_env->GetStringLength( (jstring)jo_stack_trace.get() );
                auto_ptr< rtl_mem > ustr_mem(
                    rtl_mem::allocate( sizeof (rtl_uString) + (len * sizeof (sal_Unicode)) ) );
                rtl_uString * ustr = (rtl_uString *)ustr_mem.get();
                m_env->GetStringRegion( (jstring)jo_stack_trace.get(), 0, len, ustr->buffer );
                if (assert_no_exception())
                {
                    ustr->refCount = 1;
                    ustr->length = len;
                    ustr->buffer[ len ] = '\0';
                    return OUString( (rtl_uString *)ustr_mem.release(), SAL_NO_ACQUIRE );
                }
            }
        }
    }
    return OUString();
}

}

using namespace ::jni_uno;

extern "C"
{
namespace
{
//--------------------------------------------------------------------------------------------------
void SAL_CALL java_env_disposing( uno_Environment * java_env )
    SAL_THROW_EXTERN_C()
{
    ::jvmaccess::VirtualMachine * machine =
          reinterpret_cast< ::jvmaccess::VirtualMachine * >( java_env->pContext );
    java_env->pContext = 0;
    machine->release();
}
}
//##################################################################################################
void SAL_CALL uno_initEnvironment( uno_Environment * java_env )
    SAL_THROW_EXTERN_C()
{
    java_env->environmentDisposing = java_env_disposing;
    java_env->pExtEnv = 0; // no extended support
    OSL_ASSERT( 0 != java_env->pContext );

    ::jvmaccess::VirtualMachine * machine =
          reinterpret_cast< ::jvmaccess::VirtualMachine * >( java_env->pContext );
    machine->acquire();
}
//##################################################################################################
void SAL_CALL uno_ext_getMapping(
    uno_Mapping ** ppMapping, uno_Environment * pFrom, uno_Environment * pTo )
    SAL_THROW_EXTERN_C()
{
    OSL_ASSERT( 0 != ppMapping && 0 != pFrom && 0 != pTo );
    if (0 != *ppMapping)
    {
        (*(*ppMapping)->release)( *ppMapping );
        *ppMapping = 0;
    }

    OSL_ASSERT( JNI_FALSE == sal_False );
    OSL_ASSERT( JNI_TRUE == sal_True );
    OSL_ASSERT( sizeof (jboolean) == sizeof (sal_Bool) );
    OSL_ASSERT( sizeof (jchar) == sizeof (sal_Unicode) );
    OSL_ASSERT( sizeof (jdouble) == sizeof (double) );
    OSL_ASSERT( sizeof (jfloat) == sizeof (float) );
    OSL_ASSERT( sizeof (jbyte) == sizeof (sal_Int8) );
    OSL_ASSERT( sizeof (jshort) == sizeof (sal_Int16) );
    OSL_ASSERT( sizeof (jint) == sizeof (sal_Int32) );
    OSL_ASSERT( sizeof (jlong) == sizeof (sal_Int64) );
    if ((JNI_FALSE == sal_False) &&
        (JNI_TRUE == sal_True) &&
        (sizeof (jboolean) == sizeof (sal_Bool)) &&
        (sizeof (jchar) == sizeof (sal_Unicode)) &&
        (sizeof (jdouble) == sizeof (double)) &&
        (sizeof (jfloat) == sizeof (float)) &&
        (sizeof (jbyte) == sizeof (sal_Int8)) &&
        (sizeof (jshort) == sizeof (sal_Int16)) &&
        (sizeof (jint) == sizeof (sal_Int32)) &&
        (sizeof (jlong) == sizeof (sal_Int64)))
    {
        OUString const & from_env_typename = *reinterpret_cast< OUString const * >(
            &pFrom->pTypeName );
        OUString const & to_env_typename = *reinterpret_cast< OUString const * >(
            &pTo->pTypeName );

        uno_Mapping * mapping = 0;

        try
        {
            if (from_env_typename.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_LB_JAVA) ) &&
                to_env_typename.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_LB_UNO) ))
            {
                Bridge * bridge = new Bridge( pFrom, pTo->pExtEnv, true ); // ref count = 1
                mapping = &bridge->m_java2uno;
                uno_registerMapping(
                    &mapping, Bridge_free, pFrom, (uno_Environment *)pTo->pExtEnv, 0 );
            }
            else if (from_env_typename.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_LB_UNO) ) &&
                     to_env_typename.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_LB_JAVA) ))
            {
                Bridge * bridge = new Bridge( pTo, pFrom->pExtEnv, false ); // ref count = 1
                mapping = &bridge->m_uno2java;
                uno_registerMapping(
                    &mapping, Bridge_free, (uno_Environment *)pFrom->pExtEnv, pTo, 0 );
            }
        }
        catch (BridgeRuntimeError & err)
        {
#if OSL_DEBUG_LEVEL > 0
            OString cstr_msg(
                OUStringToOString(
                    OUSTR("[jni_uno bridge error] ") + err.m_message, RTL_TEXTENCODING_ASCII_US ) );
            OSL_ENSURE( 0, cstr_msg.getStr() );
#endif
        }
        catch (::jvmaccess::VirtualMachine::AttachGuard::CreationException &)
        {
            OSL_ENSURE( 0, "[jni_uno bridge error] attaching current thread to java failed!" );
        }

        *ppMapping = mapping;
    }
}
//##################################################################################################
sal_Bool SAL_CALL component_canUnload( TimeValue * pTime )
    SAL_THROW_EXTERN_C()
{
    return (*g_moduleCount.canUnload)( &g_moduleCount, pTime );
}
}
