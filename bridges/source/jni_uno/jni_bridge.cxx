/*************************************************************************
 *
 *  $RCSfile: jni_bridge.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: dbo $ $Date: 2002-11-18 13:07:52 $
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

#include <rtl/unload.h>
#include <rtl/strbuf.hxx>
#include <uno/lbnames.h>


using namespace ::std;
using namespace ::rtl;
using namespace ::osl;
using namespace ::jni_bridge;

namespace
{
extern "C"
{

//--------------------------------------------------------------------------------------------------
void SAL_CALL jni_Mapping_acquire( uno_Mapping * mapping )
    SAL_THROW_EXTERN_C()
{
    jni_Mapping const * that = static_cast< jni_Mapping const * >( mapping );
    that->m_bridge->acquire();
}
//--------------------------------------------------------------------------------------------------
void SAL_CALL jni_Mapping_release( uno_Mapping * mapping )
    SAL_THROW_EXTERN_C()
{
    jni_Mapping const * that = static_cast< jni_Mapping const * >( mapping );
    that->m_bridge->release();
}
//--------------------------------------------------------------------------------------------------
void SAL_CALL jni_Mapping_java2uno(
    uno_Mapping * mapping, void ** ppOut,
    void * pIn, typelib_InterfaceTypeDescription * td )
    SAL_THROW_EXTERN_C()
{
    uno_Interface ** ppUnoI = (uno_Interface **)ppOut;
    jobject javaI = (jobject)pIn;
    
    OSL_ASSERT( sizeof (void *) == sizeof (jobject) );
    OSL_ENSURE( ppUnoI && td, "### null ptr!" );
    
    if (0 != *ppUnoI)
    {
        uno_Interface * pUnoI = *(uno_Interface **)ppUnoI;
        (*pUnoI->release)( pUnoI );
        *ppUnoI = 0;
    }
    if (0 != javaI)
    {
        try
        {
            jni_Mapping const * that = static_cast< jni_Mapping const * >( mapping );
            jni_Bridge const * bridge = that->m_bridge;
            JNI_attach attach( bridge->m_java_env );
            JNI_type_info const * info =
                bridge->m_jni_info->get_type_info( attach, (typelib_TypeDescription *)td );
            *ppUnoI = bridge->map_java2uno( attach, javaI, info );
        }
        catch (BridgeRuntimeError & err)
        {
#ifdef _DEBUG
            OString cstr_msg(
                OUStringToOString(
                    OUSTR("[jni_uno bridge error] ") + err.m_message, RTL_TEXTENCODING_ASCII_US ) );
            OSL_ENSURE( 0, cstr_msg.getStr() );
#endif
        }
    }
}
//--------------------------------------------------------------------------------------------------
void SAL_CALL jni_Mapping_uno2java(
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
        jni_Mapping const * that = static_cast< jni_Mapping const * >( mapping );
        jni_Bridge const * bridge = that->m_bridge;
        JNI_attach attach( bridge->m_java_env );
        
        if (0 != *ppJavaI)
        {
            attach->DeleteGlobalRef( *ppJavaI );
            *ppJavaI = 0;
        }
        if (0 != pUnoI)
        {
            JNI_type_info const * info =
                bridge->m_jni_info->get_type_info( attach, (typelib_TypeDescription *)td );
            jobject jlocal = bridge->map_uno2java( attach, pUnoI, info );
            *ppJavaI = attach->NewGlobalRef( jlocal );
            attach->DeleteLocalRef( jlocal );
        }
    }
    catch (BridgeRuntimeError & err)
    {
#ifdef _DEBUG
        OString cstr_msg(
            OUStringToOString(
                OUSTR("[jni_uno bridge error] ") + err.m_message, RTL_TEXTENCODING_ASCII_US ) );
        OSL_ENSURE( 0, cstr_msg.getStr() );
#endif
    }
}
//__________________________________________________________________________________________________
void SAL_CALL jni_Bridge_free( uno_Mapping * mapping )
    SAL_THROW_EXTERN_C()
{
    jni_Mapping * that = static_cast< jni_Mapping * >( mapping );
    delete that->m_bridge;
}

}
}

namespace jni_bridge
{

//==================================================================================================
rtl_StandardModuleCount g_moduleCount = MODULE_COUNT_INIT;

//__________________________________________________________________________________________________
void jni_Bridge::acquire() const SAL_THROW( () )
{
    if (1 == osl_incrementInterlockedCount( &m_ref ))
    {
        if (m_registered_java2uno)
        {
            uno_Mapping * mapping = const_cast< jni_Mapping * >( &m_java2uno );
            ::uno_registerMapping(
                &mapping, jni_Bridge_free,
                m_java_env, (uno_Environment *)m_uno_env, 0 );
        }
        else
        {
            uno_Mapping * mapping = const_cast< jni_Mapping * >( &m_uno2java );
            ::uno_registerMapping(
                &mapping, jni_Bridge_free,
                (uno_Environment *)m_uno_env, m_java_env, 0 );
        }
    }
}
//__________________________________________________________________________________________________
void jni_Bridge::release() const SAL_THROW( () )
{
    if (! osl_decrementInterlockedCount( &m_ref ))
    {
        ::uno_revokeMapping(
            m_registered_java2uno
            ? const_cast< jni_Mapping * >( &m_java2uno )
            : const_cast< jni_Mapping * >( &m_uno2java ) );
    }
}
//__________________________________________________________________________________________________
jni_Bridge::jni_Bridge(
    uno_Environment * java_env, uno_ExtEnvironment * uno_env, bool register_java2uno )
    SAL_THROW( () )
        : m_ref( 1 ),
          m_uno_env( uno_env ),
          m_java_env( java_env ),
          m_registered_java2uno( register_java2uno )
{
    OSL_ASSERT( 0 != m_java_env && 0 != m_uno_env );
    (*((uno_Environment *)m_uno_env)->acquire)( (uno_Environment *)m_uno_env );
    (*m_java_env->acquire)( m_java_env );
    JavaVMContext * jvm_context = reinterpret_cast< JavaVMContext * >( java_env->pContext );
    m_jni_info = reinterpret_cast< JNI_info * >( jvm_context->m_extra );
    
    // java2uno
    m_java2uno.acquire = jni_Mapping_acquire;
    m_java2uno.release = jni_Mapping_release;
    m_java2uno.mapInterface = (uno_MapInterfaceFunc)jni_Mapping_java2uno;
    m_java2uno.m_bridge = this;
    // uno2java
    m_uno2java.acquire = jni_Mapping_acquire;
    m_uno2java.release = jni_Mapping_release;
    m_uno2java.mapInterface = jni_Mapping_uno2java;
    m_uno2java.m_bridge = this;
    
    (*g_moduleCount.modCnt.acquire)( &g_moduleCount.modCnt );
}
//__________________________________________________________________________________________________
jni_Bridge::~jni_Bridge()
    SAL_THROW( () )
{
    (*m_java_env->release)( m_java_env );
    (*((uno_Environment *)m_uno_env)->release)( (uno_Environment *)m_uno_env );
    
    (*g_moduleCount.modCnt.release)( &g_moduleCount.modCnt );
}

//##################################################################################################

//__________________________________________________________________________________________________
JNI_attach::JNI_attach( uno_Environment * java_env, JNIEnv * jni_env ) SAL_THROW( () )
    : m_thread_id( ::osl::Thread::getCurrentIdentifier() ),
      m_env( jni_env ),
      m_detach( false )
{
    OSL_ASSERT( 0 == rtl_ustr_ascii_compare( java_env->pTypeName->buffer, "java" ) );
    m_context = (JavaVMContext *)java_env->pContext;
    m_vm = m_context->_pJavaVM;
    m_jni_info = (JNI_info const *)m_context->m_extra;
    
    MutexGuard guard( m_context->_mutex );
    pair< JavaVMContext::t_map::iterator, bool > insertion(
        m_context->_registeredThreadMap.insert(
            JavaVMContext::t_map::value_type(
                m_thread_id, JavaVMContext::t_entry( 1, m_env ) ) ) );
    m_revoke = insertion.second;
}
//__________________________________________________________________________________________________
JNI_attach::JNI_attach( uno_Environment * java_env )
    : m_thread_id( ::osl::Thread::getCurrentIdentifier() )
{
    OSL_ASSERT( 0 == rtl_ustr_ascii_compare( java_env->pTypeName->buffer, "java" ) );
    m_context = (JavaVMContext *)java_env->pContext;
    m_vm = m_context->_pJavaVM;
    m_jni_info = (JNI_info const *)m_context->m_extra;
    
    MutexGuard guard( m_context->_mutex );
    pair< JavaVMContext::t_map::iterator, bool > insertion(
        m_context->_registeredThreadMap.insert(
            JavaVMContext::t_map::value_type(
                m_thread_id, JavaVMContext::t_entry( 1, 0 ) ) ) );
    if (insertion.second)
    {
        m_revoke = true;
        jint res = m_vm->AttachCurrentThread( (void **)&m_env, 0 );
        insertion.first->second.second = m_env;
        if (0 < res)
        {
            m_detach = false;
            throw BridgeRuntimeError( OUSTR("AttachCurrentThread() failed!") );
        }
        m_detach = true;
    }
    else
    {
        m_revoke = false;
        m_env = insertion.first->second.second;
        m_detach = false;
    }
}
//__________________________________________________________________________________________________
JNI_attach::~JNI_attach() SAL_THROW( () )
{
    if (m_revoke)
    {
        MutexGuard guard( m_context->_mutex );
        size_t erased = m_context->_registeredThreadMap.erase( m_thread_id );
        OSL_ASSERT( 1 == erased );
    }
    if (m_detach)
    {
        jint res = m_vm->DetachCurrentThread();
        OSL_ASSERT( 0 == res );
    }
}
//__________________________________________________________________________________________________
void JNI_attach::throw_bridge_error() const
{
    JLocalAutoRef jo_exc( *this, m_env->ExceptionOccurred() );
    m_env->ExceptionClear();
    OSL_ASSERT( jo_exc.is() );
    if (! jo_exc.is())
    {
        throw BridgeRuntimeError(
            OUSTR("throw_bridge_error(): java exception occured, but not available!?") );
    }
    
    // call toString(); don't rely on m_jni_info
    jclass jo_class = m_env->FindClass( "java/lang/Object" );
    if (JNI_FALSE != m_env->ExceptionCheck())
    {
        m_env->ExceptionClear();
        throw BridgeRuntimeError(
            OUSTR("essential java problem: cannot get class java.lang.Object!") );
    }
    JLocalAutoRef jo_Object( *this, jo_class );
    // method Object.toString()
    jmethodID method_Object_toString = m_env->GetMethodID(
        (jclass)jo_Object.get(), "toString", "()Ljava/lang/String;" );
    if (JNI_FALSE != m_env->ExceptionCheck())
    {
        m_env->ExceptionClear();
        throw BridgeRuntimeError(
            OUSTR("essential java problem: cannot get method id of java.lang.Object.toString()!") );
    }
    OSL_ASSERT( 0 != method_Object_toString );
    
    JLocalAutoRef jo_descr(
        *this, m_env->CallObjectMethodA( jo_exc.get(), method_Object_toString, 0 ) );
    if (m_env->ExceptionCheck()) // no chance at all
    {
        m_env->ExceptionClear();
        throw BridgeRuntimeError(
            OUSTR("throw_bridge_error(): error examining exception object!") );
    }
    
    OSL_ASSERT( sizeof (sal_Unicode) == sizeof (jchar) );
    jsize len = m_env->GetStringLength( (jstring)jo_descr.get() );
    auto_ptr< rtl_mem > ustr_mem(
        rtl_mem::allocate( sizeof (rtl_uString) + (len * sizeof (sal_Unicode)) ) );
    rtl_uString * ustr = (rtl_uString *)ustr_mem.get();
    m_env->GetStringRegion( (jstring)jo_descr.get(), 0, len, ustr->buffer );
    if (m_env->ExceptionCheck())
    {
        m_env->ExceptionClear();
        throw BridgeRuntimeError(
            OUSTR("throw_bridge_error(): invalid string object!") );
    }
    ustr->refCount = 1;
    ustr->length = len;
    ustr->buffer[ len ] = '\0';
    
    throw BridgeRuntimeError( OUString( (rtl_uString *)ustr_mem.release(), SAL_NO_ACQUIRE ) );
}

}

using namespace ::jni_bridge;

extern "C"
{
namespace
{
//--------------------------------------------------------------------------------------------------
void SAL_CALL java_env_disposing( uno_Environment * java_env )
    SAL_THROW_EXTERN_C()
{
    JavaVMContext * context = reinterpret_cast< JavaVMContext * >( java_env->pContext );
    delete reinterpret_cast< JNI_info * >( context->m_extra );
    java_env->pContext = 0;
    delete context;
}
}
//##################################################################################################
void SAL_CALL uno_initEnvironment( uno_Environment * java_env )
    SAL_THROW_EXTERN_C()
{
    java_env->environmentDisposing = java_env_disposing;
    java_env->pExtEnv = 0; // no extended support
    OSL_ASSERT( 0 != java_env->pContext );
    try
    {
        ((JavaVMContext *)java_env->pContext)->m_extra = new JNI_info( java_env );
    }
    catch (BridgeRuntimeError & err)
    {
#ifdef _DEBUG
        OStringBuffer buf( 64 );
        buf.append( RTL_CONSTASCII_STRINGPARAM("### error initializing (jni) java environment: ") );
        buf.append( OUStringToOString( err.m_message, RTL_TEXTENCODING_ASCII_US ) );
        OString msg( buf.makeStringAndClear() );
        OSL_ENSURE( 0, msg.getStr() );
#endif
    }
}
//##################################################################################################
void SAL_CALL uno_ext_getMapping(
    uno_Mapping ** ppMapping, uno_Environment * pFrom, uno_Environment * pTo )
    SAL_THROW_EXTERN_C()
{
    OSL_ASSERT( 0 != ppMapping && 0 != pFrom && 0 != pTo );
    if (*ppMapping)
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
        
        if (from_env_typename.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_LB_JAVA) ) &&
            to_env_typename.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_LB_UNO) ))
        {
            jni_Bridge * bridge = new jni_Bridge( pFrom, pTo->pExtEnv, true ); // ref count = 1
            mapping = &bridge->m_java2uno;
            uno_registerMapping(
                &mapping, jni_Bridge_free,
                pFrom, (uno_Environment *)pTo->pExtEnv, 0 );
        }
        else if (from_env_typename.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_LB_UNO) ) &&
                 to_env_typename.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_LB_JAVA) ))
        {
            jni_Bridge * bridge = new jni_Bridge( pTo, pFrom->pExtEnv, false ); // ref count = 1
            mapping = &bridge->m_uno2java;
            uno_registerMapping(
                &mapping, jni_Bridge_free,
                (uno_Environment *)pFrom->pExtEnv, pTo, 0 );
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
