/*************************************************************************
 *
 *  $RCSfile: jni_uno2java.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: vg $ $Date: 2003-04-15 16:27:28 $
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

#if defined _MSC_VER
#include <malloc.h>
#else
#include <alloca.h>
#endif

#include "rtl/ustrbuf.hxx"

#include "jni_bridge.h"


using namespace ::std;
using namespace ::rtl;

namespace
{
extern "C"
{
//--------------------------------------------------------------------------------------------------
void SAL_CALL UNO_proxy_free( uno_ExtEnvironment * env, void * proxy )
    SAL_THROW_EXTERN_C();
//--------------------------------------------------------------------------------------------------
void SAL_CALL UNO_proxy_acquire( uno_Interface * pUnoI )
    SAL_THROW_EXTERN_C();
//--------------------------------------------------------------------------------------------------
void SAL_CALL UNO_proxy_release( uno_Interface * pUnoI )
    SAL_THROW_EXTERN_C();
//--------------------------------------------------------------------------------------------------
void SAL_CALL UNO_proxy_dispatch(
    uno_Interface * pUnoI, typelib_TypeDescription const * member_td,
    void * uno_ret, void * uno_args[], uno_Any ** uno_exc )
    SAL_THROW_EXTERN_C();
}
}

namespace jni_uno
{

//__________________________________________________________________________________________________
void Bridge::handle_java_exc(
    JNI_context const & jni, JLocalAutoRef const & jo_exc, uno_Any * uno_exc ) const
{
    OSL_ASSERT( jo_exc.is() );
    if (! jo_exc.is())
    {
        throw BridgeRuntimeError(
            OUSTR("java exception occured, but no java exception available!?") +
            jni.get_stack_trace() );
    }

    JLocalAutoRef jo_class( jni, jni->GetObjectClass( jo_exc.get() ) );
    JLocalAutoRef jo_class_name(
        jni, jni->CallObjectMethodA( jo_class.get(), m_jni_info->m_method_Class_getName, 0 ) );
    jni.ensure_no_exception();
    OUString exc_name( jstring_to_oustring( jni, (jstring)jo_class_name.get() ) );

    ::com::sun::star::uno::TypeDescription td( exc_name.pData );
    if (!td.is() || (typelib_TypeClass_EXCEPTION != td.get()->eTypeClass))
    {
        // call toString()
        JLocalAutoRef jo_descr(
            jni, jni->CallObjectMethodA(
                jo_exc.get(), m_jni_info->m_method_Object_toString, 0 ) );
        jni.ensure_no_exception();
        OUStringBuffer buf( 128 );
        buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("non-UNO exception occurred: ") );
        buf.append( jstring_to_oustring( jni, (jstring)jo_descr.get() ) );
        buf.append( jni.get_stack_trace( jo_exc.get() ) );
        throw BridgeRuntimeError( buf.makeStringAndClear() );
    }

    auto_ptr< rtl_mem > uno_data( rtl_mem::allocate( td.get()->nSize ) );
    jvalue val;
    val.l = jo_exc.get();
    map_to_uno(
        jni, uno_data.get(), val, td.get()->pWeakRef, 0,
        false /* no assign */, false /* no out param */ );

#if OSL_DEBUG_LEVEL > 0
    // patch Message, append stack trace
    reinterpret_cast< ::com::sun::star::uno::Exception * >( uno_data.get() )->Message +=
        jni.get_stack_trace( jo_exc.get() );
#endif

    typelib_typedescriptionreference_acquire( td.get()->pWeakRef );
    uno_exc->pType = td.get()->pWeakRef;
    uno_exc->pData = uno_data.release();

#if OSL_DEBUG_LEVEL > 1
    OUStringBuffer trace_buf( 128 );
    trace_buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("exception occured uno->java: [") );
    trace_buf.append( exc_name );
    trace_buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("] ") );
    trace_buf.append(
        reinterpret_cast< ::com::sun::star::uno::Exception const * >( uno_exc->pData )->Message );
    OString cstr_trace(
        OUStringToOString( trace_buf.makeStringAndClear(), RTL_TEXTENCODING_ASCII_US ) );
    OSL_TRACE( cstr_trace.getStr() );
#endif
}
//__________________________________________________________________________________________________
void Bridge::call_java(
    jobject javaI, JNI_interface_type_info const * info, sal_Int32 function_pos,
    typelib_TypeDescriptionReference * return_type,
    typelib_MethodParameter * params, sal_Int32 nParams,
    void * uno_ret, void * uno_args [], uno_Any ** uno_exc ) const
{
    // determine exact interface type info
    while (0 != info->m_base)
    {
        typelib_InterfaceTypeDescription * base_td =
            (typelib_InterfaceTypeDescription *)info->m_base->m_td.get();
        sal_Int32 base_functions = base_td->nMapFunctionIndexToMemberIndex;
        if (function_pos >= base_functions)
        {
            function_pos -= base_functions;
            break;
        }
        info = static_cast< JNI_interface_type_info const * >( info->m_base );
    }

    JNI_guarded_context jni(
        m_jni_info, reinterpret_cast< ::jvmaccess::VirtualMachine * >( m_java_env->pContext ) );

    // prepare java args, save param td
    jvalue * java_args = (jvalue *)alloca( sizeof (jvalue) * nParams );

    sal_Int32 nPos;
    for ( nPos = 0; nPos < nParams; ++nPos )
    {
        try
        {
            typelib_MethodParameter const & param = params[ nPos ];
            java_args[ nPos ].l = 0; // if out: build up array[ 1 ]
            map_to_java(
                jni, &java_args[ nPos ],
                uno_args[ nPos ],
                param.pTypeRef, 0,
                sal_False != param.bIn /* convert uno value */,
                sal_False != param.bOut /* build up array[ 1 ] */ );
        }
        catch (...)
        {
            // cleanup
            for ( sal_Int32 n = 0; n < nPos; ++n )
            {
                typelib_MethodParameter const & param = params[ n ];
                if (param.bOut || typelib_TypeClass_DOUBLE < param.pTypeRef->eTypeClass)
                    jni->DeleteLocalRef( java_args[ n ].l );
            }
            throw;
        }
    }

    jmethodID method_id = info->m_methods[ function_pos ];

#if OSL_DEBUG_LEVEL > 1
    OUStringBuffer trace_buf( 128 );
    trace_buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("calling ") );
    JLocalAutoRef jo_method( jni, jni->ToReflectedMethod( info->m_class, method_id, JNI_FALSE ) );
    jni.ensure_no_exception();
    JLocalAutoRef jo_descr(
        jni, jni->CallObjectMethodA( jo_method.get(), m_jni_info->m_method_Object_toString, 0 ) );
    jni.ensure_no_exception();
    trace_buf.append( jstring_to_oustring( jni, (jstring) jo_descr.get() ) );
    trace_buf.appendAscii( RTL_CONSTASCII_STRINGPARAM(" on ") );
    jo_descr.reset(
        jni->CallObjectMethodA( javaI, m_jni_info->m_method_Object_toString, 0 ) );
    jni.ensure_no_exception();
    trace_buf.append( jstring_to_oustring( jni, (jstring) jo_descr.get() ) );
    trace_buf.appendAscii( RTL_CONSTASCII_STRINGPARAM(" (") );
    JLocalAutoRef jo_class( jni, jni->GetObjectClass( javaI ) );
    jo_descr.reset(
        jni->CallObjectMethodA( jo_class.get(), m_jni_info->m_method_Object_toString, 0 ) );
    jni.ensure_no_exception();
    trace_buf.append( jstring_to_oustring( jni, (jstring) jo_descr.get() ) );
    trace_buf.appendAscii( RTL_CONSTASCII_STRINGPARAM(")") );
    OString cstr_trace(
        OUStringToOString( trace_buf.makeStringAndClear(), RTL_TEXTENCODING_ASCII_US ) );
    OSL_TRACE( cstr_trace.getStr() );
#endif

    // complex return value
    JLocalAutoRef java_ret( jni );

    switch (return_type->eTypeClass)
    {
    case typelib_TypeClass_VOID:
        jni->CallVoidMethodA( javaI, method_id, java_args );
        break;
    case typelib_TypeClass_CHAR:
        *(sal_Unicode *)uno_ret = jni->CallCharMethodA( javaI, method_id, java_args );
        break;
    case typelib_TypeClass_BOOLEAN:
        *(sal_Bool *)uno_ret = jni->CallBooleanMethodA( javaI, method_id, java_args );
        break;
    case typelib_TypeClass_BYTE:
        *(sal_Int8 *)uno_ret = jni->CallByteMethodA( javaI, method_id, java_args );
        break;
    case typelib_TypeClass_SHORT:
    case typelib_TypeClass_UNSIGNED_SHORT:
        *(sal_Int16 *)uno_ret = jni->CallShortMethodA( javaI, method_id, java_args );
        break;
    case typelib_TypeClass_LONG:
    case typelib_TypeClass_UNSIGNED_LONG:
        *(sal_Int32 *)uno_ret = jni->CallIntMethodA( javaI, method_id, java_args );
        break;
    case typelib_TypeClass_HYPER:
    case typelib_TypeClass_UNSIGNED_HYPER:
        *(sal_Int64 *)uno_ret = jni->CallLongMethodA( javaI, method_id, java_args );
        break;
    case typelib_TypeClass_FLOAT:
        *(float *)uno_ret = jni->CallFloatMethodA( javaI, method_id, java_args );
        break;
    case typelib_TypeClass_DOUBLE:
        *(double *)uno_ret = jni->CallDoubleMethodA( javaI, method_id, java_args );
        break;
    default:
        java_ret.reset( jni->CallObjectMethodA( javaI, method_id, java_args ) );
        break;
    }

    if (jni->ExceptionCheck())
    {
        JLocalAutoRef jo_exc( jni, jni->ExceptionOccurred() );
        jni->ExceptionClear();

        // release temp java local refs
        for ( nPos = 0; nPos < nParams; ++nPos )
        {
            typelib_MethodParameter const & param = params[ nPos ];
            if (param.bOut || typelib_TypeClass_DOUBLE < param.pTypeRef->eTypeClass)
                jni->DeleteLocalRef( java_args[ nPos ].l );
        }

        handle_java_exc( jni, jo_exc, *uno_exc );
    }
    else // no exception
    {
        for ( nPos = 0; nPos < nParams; ++nPos )
        {
            typelib_MethodParameter const & param = params[ nPos ];
            if (param.bOut)
            {
                try
                {
                    map_to_uno(
                        jni, uno_args[ nPos ], java_args[ nPos ], param.pTypeRef, 0,
                        sal_False != param.bIn /* assign if inout */, true /* out param */ );
                }
                catch (...)
                {
                    // cleanup uno pure out
                    for ( sal_Int32 n = 0; n < nPos; ++n )
                    {
                        typelib_MethodParameter const & param = params[ n ];
                        if (! param.bIn)
                            uno_type_destructData( uno_args[ n ], param.pTypeRef, 0 );
                    }
                    // cleanup java temp local refs
                    for ( ; nPos < nParams; ++nPos )
                    {
                        typelib_MethodParameter const & param = params[ nPos ];
                        if (param.bOut || typelib_TypeClass_DOUBLE < param.pTypeRef->eTypeClass)
                            jni->DeleteLocalRef( java_args[ nPos ].l );
                    }
                    throw;
                }
                jni->DeleteLocalRef( java_args[ nPos ].l );
            }
            else // pure temp in param
            {
                if (typelib_TypeClass_DOUBLE < param.pTypeRef->eTypeClass)
                    jni->DeleteLocalRef( java_args[ nPos ].l );
            }
        }

        // return value
        if (typelib_TypeClass_DOUBLE < return_type->eTypeClass)
        {
            try
            {
                jvalue val;
                val.l = java_ret.get();
                map_to_uno(
                    jni, uno_ret, val, return_type, 0,
                    false /* no assign */, false /* no out param */ );
            }
            catch (...)
            {
                // cleanup uno pure out
                for ( sal_Int32 nPos = 0; nPos < nParams; ++nPos )
                {
                    typelib_MethodParameter const & param = params[ nPos ];
                    if (! param.bIn)
                        uno_type_destructData( uno_args[ nPos ], param.pTypeRef, 0 );
                }
                throw;
            }
        } // else: already set integral uno return value

        // no exception occured
        *uno_exc = 0;
    }
}

//==== a uno proxy wrapping a java interface =======================================================
struct UNO_proxy : public uno_Interface
{
    mutable oslInterlockedCount         m_ref;
    Bridge const *                      m_bridge;

    // mapping information
    jobject                             m_javaI;
    jstring                             m_jo_oid;
    OUString                            m_oid;
    JNI_interface_type_info const *     m_type_info;

    inline void acquire() const;
    inline void release() const;

    // ctor
    inline UNO_proxy(
        JNI_context const & jni, Bridge const * bridge,
        jobject javaI, jstring jo_oid, OUString const & oid, JNI_interface_type_info const * info );
};
//__________________________________________________________________________________________________
inline UNO_proxy::UNO_proxy(
    JNI_context const & jni, Bridge const * bridge,
    jobject javaI, jstring jo_oid, OUString const & oid, JNI_interface_type_info const * info )
    : m_ref( 1 ),
      m_oid( oid ),
      m_type_info( info )
{
    JNI_info const * jni_info = bridge->m_jni_info;
    JLocalAutoRef jo_string_array(
        jni, jni->NewObjectArray( 1, jni_info->m_class_String, jo_oid ) );
    jni.ensure_no_exception();
    jvalue args[ 3 ];
    args[ 0 ].l = javaI;
    args[ 1 ].l = jo_string_array.get();
    args[ 2 ].l = info->m_type;
    jobject jo_iface = jni->CallObjectMethodA(
        jni_info->m_object_java_env, jni_info->m_method_IEnvironment_registerInterface, args );
    jni.ensure_no_exception();

    m_javaI = jni->NewGlobalRef( jo_iface );
    m_jo_oid = (jstring)jni->NewGlobalRef( jo_oid );
    bridge->acquire();
    m_bridge = bridge;

    // uno_Interface
    uno_Interface::acquire = UNO_proxy_acquire;
    uno_Interface::release = UNO_proxy_release;
    uno_Interface::pDispatcher = UNO_proxy_dispatch;
}
//__________________________________________________________________________________________________
inline void UNO_proxy::acquire() const
{
    if (1 == osl_incrementInterlockedCount( &m_ref ))
    {
        // rebirth of proxy zombie
        void * that = const_cast< UNO_proxy * >( this );
        // register at uno env
        (*m_bridge->m_uno_env->registerProxyInterface)(
            m_bridge->m_uno_env, &that,
            UNO_proxy_free, m_oid.pData,
            (typelib_InterfaceTypeDescription *)m_type_info->m_td.get() );
#if OSL_DEBUG_LEVEL > 1
        OSL_ASSERT( this == (void const * const)that );
#endif
    }
}
//__________________________________________________________________________________________________
inline void UNO_proxy::release() const
{
    if (0 == osl_decrementInterlockedCount( &m_ref ))
    {
        // revoke from uno env on last release
        (*m_bridge->m_uno_env->revokeInterface)(
            m_bridge->m_uno_env, const_cast< UNO_proxy * >( this ) );
    }
}

//##################################################################################################

//__________________________________________________________________________________________________
uno_Interface * Bridge::map_to_uno(
    JNI_context const & jni,
    jobject javaI, JNI_interface_type_info const * info ) const
{
    JLocalAutoRef jo_oid( jni, compute_oid( jni, javaI ) );
    OUString oid( jstring_to_oustring( jni, (jstring)jo_oid.get() ) );

    uno_Interface * pUnoI = 0;
    (*m_uno_env->getRegisteredInterface)(
        m_uno_env, (void **)&pUnoI,
        oid.pData, (typelib_InterfaceTypeDescription *)info->m_td.get() );

    if (0 == pUnoI) // no existing interface, register new proxy
    {
        // refcount initially 1
        pUnoI = new UNO_proxy(
            jni, const_cast< Bridge * >( this ),
            javaI, (jstring)jo_oid.get(), oid, info );

        (*m_uno_env->registerProxyInterface)(
            m_uno_env, (void **)&pUnoI,
            UNO_proxy_free,
            oid.pData, (typelib_InterfaceTypeDescription *)info->m_td.get() );
    }
    return pUnoI;
}

}

using namespace ::jni_uno;

namespace
{
extern "C"
{

//--------------------------------------------------------------------------------------------------
void SAL_CALL UNO_proxy_free( uno_ExtEnvironment * env, void * proxy )
    SAL_THROW_EXTERN_C()
{
    UNO_proxy const * that = reinterpret_cast< UNO_proxy const * >( proxy );
    Bridge const * bridge = that->m_bridge;

    OSL_ASSERT( env == bridge->m_uno_env );
#if OSL_DEBUG_LEVEL > 1
    OString cstr_msg(
        OUStringToOString(
            OUSTR("freeing binary uno proxy: ") + that->m_oid, RTL_TEXTENCODING_ASCII_US ) );
    OSL_TRACE( cstr_msg.getStr() );
#endif

    try
    {
        JNI_guarded_context jni(
            bridge->m_jni_info,
            reinterpret_cast< ::jvmaccess::VirtualMachine * >( bridge->m_java_env->pContext ) );

        jni->DeleteGlobalRef( that->m_javaI );
        jni->DeleteGlobalRef( that->m_jo_oid );
    }
    catch (BridgeRuntimeError & err)
    {
#if OSL_DEBUG_LEVEL > 0
        OString cstr_msg( OUStringToOString( err.m_message, RTL_TEXTENCODING_ASCII_US ) );
        OSL_ENSURE( 0, cstr_msg.getStr() );
#endif
    }
    catch (::jvmaccess::VirtualMachine::AttachGuard::CreationException &)
    {
        OSL_ENSURE( 0, "[jni_uno bridge error] attaching current thread to java failed!" );
    }

    bridge->release();
#if OSL_DEBUG_LEVEL > 1
    *(int *)that = 0xdeadcafe;
#endif
    delete that;
}
//--------------------------------------------------------------------------------------------------
void SAL_CALL UNO_proxy_acquire( uno_Interface * pUnoI )
    SAL_THROW_EXTERN_C()
{
    UNO_proxy const * that = static_cast< UNO_proxy const * >( pUnoI );
    that->acquire();
}
//--------------------------------------------------------------------------------------------------
void SAL_CALL UNO_proxy_release( uno_Interface * pUnoI )
    SAL_THROW_EXTERN_C()
{
    UNO_proxy const * that = static_cast< UNO_proxy const * >( pUnoI );
    that->release();
}

//--------------------------------------------------------------------------------------------------
void SAL_CALL UNO_proxy_dispatch(
    uno_Interface * pUnoI, typelib_TypeDescription const * member_td,
    void * uno_ret, void * uno_args [], uno_Any ** uno_exc )
    SAL_THROW_EXTERN_C()
{
    UNO_proxy const * that = static_cast< UNO_proxy const * >( pUnoI );
    Bridge const * bridge = that->m_bridge;

#if OSL_DEBUG_LEVEL > 1
    OUStringBuffer trace_buf( 64 );
    trace_buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("uno->java call: ") );
    trace_buf.append( *reinterpret_cast< OUString const * >( &member_td->pTypeName ) );
    trace_buf.appendAscii( RTL_CONSTASCII_STRINGPARAM(" on oid ") );
    trace_buf.append( that->m_oid );
    OString cstr_msg(
        OUStringToOString( trace_buf.makeStringAndClear(), RTL_TEXTENCODING_ASCII_US ) );
    OSL_TRACE( cstr_msg.getStr() );
#endif

    try
    {
        switch (member_td->eTypeClass)
        {
        case typelib_TypeClass_INTERFACE_ATTRIBUTE:
        {
            sal_Int32 member_pos = ((typelib_InterfaceMemberTypeDescription *)member_td)->nPosition;
            typelib_InterfaceTypeDescription * iface_td =
                (typelib_InterfaceTypeDescription *)that->m_type_info->m_td.get();
            OSL_ENSURE(
                member_pos < iface_td->nAllMembers,
                "### member pos out of range!" );
            sal_Int32 function_pos = iface_td->pMapMemberIndexToFunctionIndex[ member_pos ];
            OSL_ENSURE(
                function_pos < iface_td->nMapFunctionIndexToMemberIndex,
                "### illegal function index!" );

            if (0 == uno_ret) // is setter method
            {
                typelib_MethodParameter param;
                param.pTypeRef =
                    ((typelib_InterfaceAttributeTypeDescription *)member_td)->pAttributeTypeRef;
                param.bIn = sal_True;
                param.bOut = sal_False;

                bridge->call_java(
                    that->m_javaI, that->m_type_info, function_pos +1, // get, then set method
                    bridge->m_jni_info->m_void_type.getTypeLibType(),
                    &param, 1,
                    0, uno_args, uno_exc );
            }
            else // is getter method
            {
                bridge->call_java(
                    that->m_javaI, that->m_type_info, function_pos,
                    ((typelib_InterfaceAttributeTypeDescription *)member_td)->pAttributeTypeRef,
                    0, 0, // no params
                    uno_ret, 0, uno_exc );
            }
            break;
        }
        case typelib_TypeClass_INTERFACE_METHOD:
        {
            sal_Int32 member_pos = ((typelib_InterfaceMemberTypeDescription *)member_td)->nPosition;
            typelib_InterfaceTypeDescription * iface_td =
                (typelib_InterfaceTypeDescription *)that->m_type_info->m_td.get();
            OSL_ENSURE(
                member_pos < iface_td->nAllMembers,
                "### member pos out of range!" );
            sal_Int32 function_pos =
                iface_td->pMapMemberIndexToFunctionIndex[ member_pos ];
            OSL_ENSURE(
                function_pos < iface_td->nMapFunctionIndexToMemberIndex,
                "### illegal function index!" );

            switch (function_pos)
            {
            case 0: // queryInterface()
            {
                TypeDescr demanded_td(
                    *reinterpret_cast< typelib_TypeDescriptionReference ** >( uno_args[ 0 ] ) );
                if (typelib_TypeClass_INTERFACE != demanded_td.get()->eTypeClass)
                {
                    throw BridgeRuntimeError(
                        OUSTR("queryInterface() call demands an INTERFACE type!") );
                }

                uno_Interface * pInterface = 0;
                (*bridge->m_uno_env->getRegisteredInterface)(
                    bridge->m_uno_env,
                    (void **)&pInterface, that->m_oid.pData,
                    (typelib_InterfaceTypeDescription *)demanded_td.get() );

                if (0 == pInterface)
                {
                    JNI_info const * jni_info = bridge->m_jni_info;
                    JNI_guarded_context jni(
                        jni_info,
                        reinterpret_cast< ::jvmaccess::VirtualMachine * >(
                            bridge->m_java_env->pContext ) );

                    JNI_interface_type_info const * info =
                        static_cast< JNI_interface_type_info const * >(
                            jni_info->get_type_info( jni, demanded_td.get() ) );

                    jvalue args[ 2 ];
                    args[ 0 ].l = info->m_type;
                    args[ 1 ].l = that->m_javaI;

                    JLocalAutoRef jo_ret(
                        jni, jni->CallStaticObjectMethodA(
                            jni_info->m_class_UnoRuntime,
                            jni_info->m_method_UnoRuntime_queryInterface, args ) );

                    if (jni->ExceptionCheck())
                    {
                        JLocalAutoRef jo_exc( jni, jni->ExceptionOccurred() );
                        jni->ExceptionClear();
                        bridge->handle_java_exc( jni, jo_exc, *uno_exc );
                    }
                    else
                    {
                        if (jo_ret.is())
                        {
#if OSL_DEBUG_LEVEL > 0
                            JLocalAutoRef jo_oid( jni, compute_oid( jni, jo_ret.get() ) );
                            OUString oid( jstring_to_oustring( jni, (jstring)jo_oid.get() ) );
                            OSL_ENSURE( oid.equals( that->m_oid ), "### different oids!" );
#endif
                            // refcount initially 1
                            uno_Interface * pUnoI = new UNO_proxy(
                                jni, bridge, jo_ret.get(),
                                that->m_jo_oid, that->m_oid, info );

                            (*bridge->m_uno_env->registerProxyInterface)(
                                bridge->m_uno_env, (void **)&pUnoI,
                                UNO_proxy_free, that->m_oid.pData,
                                (typelib_InterfaceTypeDescription *)info->m_td.get() );

                            uno_any_construct(
                                (uno_Any *)uno_ret, &pUnoI, demanded_td.get(), 0 );
                            (*pUnoI->release)( pUnoI );
                        }
                        else // object does not support demanded interface
                        {
                            uno_any_construct( (uno_Any *)uno_ret, 0, 0, 0 );
                        }
                        // no exception occured
                        *uno_exc = 0;
                    }
                }
                else
                {
                    uno_any_construct(
                        reinterpret_cast< uno_Any * >( uno_ret ),
                        &pInterface, demanded_td.get(), 0 );
                    (*pInterface->release)( pInterface );
                    *uno_exc = 0;
                }
                break;
            }
            case 1: // acquire this proxy
                that->acquire();
                *uno_exc = 0;
                break;
            case 2: // release this proxy
                that->release();
                *uno_exc = 0;
                break;
            default: // arbitrary method call
            {
                typelib_InterfaceMethodTypeDescription * method_td =
                    (typelib_InterfaceMethodTypeDescription *)member_td;
                bridge->call_java(
                    that->m_javaI, that->m_type_info, function_pos,
                    method_td->pReturnTypeRef,
                    method_td->pParams, method_td->nParams,
                    uno_ret, uno_args, uno_exc );
                break;
            }
            }
            break;
        }
        default:
        {
            throw BridgeRuntimeError( OUSTR("illegal member type description!") );
        }
        }
    }
    catch (BridgeRuntimeError & err)
    {
        OUStringBuffer buf( 128 );
        buf.appendAscii(
            RTL_CONSTASCII_STRINGPARAM("[jni_uno bridge error] UNO calling Java method ") );
        if (typelib_TypeClass_INTERFACE_METHOD == member_td->eTypeClass ||
            typelib_TypeClass_INTERFACE_ATTRIBUTE == member_td->eTypeClass)
        {
            buf.append(
                *reinterpret_cast< OUString const * >(
                    &reinterpret_cast< typelib_InterfaceMemberTypeDescription const * >(
                        member_td )->pMemberName ) );
        }
        buf.appendAscii( RTL_CONSTASCII_STRINGPARAM(": ") );
        buf.append( err.m_message );
        // binary identical struct
        ::com::sun::star::uno::RuntimeException exc(
            buf.makeStringAndClear(),
            ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >() );
        ::com::sun::star::uno::Type const & exc_type = ::getCppuType( &exc );
        uno_type_any_construct( *uno_exc, &exc, exc_type.getTypeLibType(), 0 );
#if OSL_DEBUG_LEVEL > 0
        OString cstr_msg( OUStringToOString( exc.Message, RTL_TEXTENCODING_ASCII_US ) );
        OSL_ENSURE( 0, cstr_msg.getStr() );
#endif
    }
    catch (::jvmaccess::VirtualMachine::AttachGuard::CreationException &)
    {
        // binary identical struct
        ::com::sun::star::uno::RuntimeException exc(
            OUSTR("[jni_uno bridge error] attaching current thread to java failed!"),
            ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >() );
        ::com::sun::star::uno::Type const & exc_type = ::getCppuType( &exc );
        uno_type_any_construct( *uno_exc, &exc, exc_type.getTypeLibType(), 0 );
#if OSL_DEBUG_LEVEL > 0
        OString cstr_msg( OUStringToOString( exc.Message, RTL_TEXTENCODING_ASCII_US ) );
        OSL_ENSURE( 0, cstr_msg.getStr() );
#endif
    }
}

}
}
