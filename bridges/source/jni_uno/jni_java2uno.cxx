/*************************************************************************
 *
 *  $RCSfile: jni_java2uno.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: dbo $ $Date: 2002-11-04 14:56:28 $
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

#ifdef _MSC_VER
#include <malloc.h>
#else
#include <alloca.h>
#endif

#include "jni_bridge.h"

#include <rtl/ustrbuf.hxx>


using namespace ::rtl;

namespace jni_bridge
{

//__________________________________________________________________________________________________
jobject jni_Bridge::map_uno2java(
    JNI_attach const & attach,
    uno_Interface * pUnoI, JNI_type_info const * info ) const
{
    // get oid
    rtl_uString * pOid = 0;
    (*m_uno_env->getObjectIdentifier)( m_uno_env, &pOid, pUnoI );
    OSL_ASSERT( 0 != pOid );
    OUString oid( pOid, SAL_NO_ACQUIRE );
    
    // getRegisteredInterface()
    JLocalAutoRef jo_oid( attach, ustring_to_jstring( attach, oid.pData ) );
    JLocalAutoRef jo_iface(
        attach, m_jni_info->java_env_getRegisteredInterface(
            attach, (jstring)jo_oid.get(), info->m_jo_type ) );
    
    if (! jo_iface.is()) // no registered iface
    {
        // register uno interface
        (*m_uno_env->registerInterface)(
            m_uno_env, reinterpret_cast< void ** >( &pUnoI ),
            oid.pData, (typelib_InterfaceTypeDescription *)info->m_td.get() );
        
        // create java and register java proxy
        jvalue args[ 5 ];
        acquire();
        args[ 0 ].j = reinterpret_cast< sal_Int64 >( this );
        (*pUnoI->acquire)( pUnoI );
        args[ 1 ].l = m_jni_info->m_object_java_env;
        args[ 2 ].j = reinterpret_cast< sal_Int64 >( pUnoI );
        typelib_typedescription_acquire( info->m_td.get() );
        args[ 3 ].j = reinterpret_cast< sal_Int64 >( info->m_td.get() );
        args[ 4 ].l = info->m_jo_type;
        args[ 5 ].l = jo_oid.get();
        jo_iface.reset(
            attach, attach->CallStaticObjectMethodA(
                m_jni_info->m_class_JNI_proxy,
                m_jni_info->m_method_JNI_proxy_create, args ) );
        attach.ensure_no_exception();
    }
    
    return jo_iface.release();
}

//##################################################################################################

//__________________________________________________________________________________________________
void jni_Bridge::handle_uno_exc( JNI_attach const & attach, uno_Any * uno_exc ) const
{
    if (typelib_TypeClass_EXCEPTION == uno_exc->pType->eTypeClass)
    {
#ifdef DEBUG
        OString cstr_msg(
            OUStringToOString(
                OUString(uno_exc->pType->pTypeName) + OUSTR(": ") +
                *(OUString const *)uno_exc->pData, RTL_TEXTENCODING_ASCII_US ) );
        OSL_TRACE( cstr_msg.getStr() );
#endif
        // signal exception
        jvalue java_exc;
        try
        {
            map_to_java( attach, &java_exc, uno_exc->pData, uno_exc->pType, 0, true, false );
        }
        catch (...)
        {
            uno_any_destruct( uno_exc, 0 );
            throw;
        }
        uno_any_destruct( uno_exc, 0 );
        JLocalAutoRef jo_exc( attach, java_exc.l );
        jint res = attach->Throw( (jthrowable)jo_exc.get() );
        if (0 != res)
        {
            OUStringBuffer buf( 64 );
            buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("throwing java exception failed: [") );
            buf.append( *reinterpret_cast< OUString const * >( &uno_exc->pType->pTypeName ) );
            buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("] ") );
            buf.append( ((::com::sun::star::uno::Exception const *)uno_exc->pData)->Message );
            throw BridgeRuntimeError( buf.makeStringAndClear() );
        }
    }
    else
    {
        throw BridgeRuntimeError(
            OUSTR("thrown exception is no uno exception: ") +
            *reinterpret_cast< OUString const * >( &uno_exc->pType->pTypeName ) );
    }
}

union largest
{
    sal_Int64 n;
    double d;
    void * p;
    uno_Any a;
};
//__________________________________________________________________________________________________
jobject jni_Bridge::call_uno(
    JNI_attach const & attach,
    uno_Interface * pUnoI, typelib_TypeDescription * member_td,
    typelib_TypeDescriptionReference * return_type /* 0 indicates void return */,
    sal_Int32 nParams, typelib_MethodParameter const * pParams,
    jobjectArray jo_args /* may be 0 */ ) const
{
    // return mem
    sal_Int32 return_size = sizeof (largest);
    if ((0 != return_type) &&
        (typelib_TypeClass_STRUCT == return_type->eTypeClass ||
         typelib_TypeClass_EXCEPTION == return_type->eTypeClass))
    {
        TypeDescr return_td( return_type );
        if (return_td.get()->nSize > sizeof (largest))
            return_size = return_td.get()->nSize;
    }
    
    char * mem = (char *)alloca(
        (nParams * sizeof (void *)) + return_size + (nParams * sizeof (largest)) );
    void ** uno_args = (void **)mem;
    void * uno_ret = (mem + (nParams * sizeof (void *)));
    largest * uno_args_mem = (largest *)(mem + (nParams * sizeof (void *)) + return_size);
    
    OSL_ASSERT( (0 == nParams) || (nParams == attach->GetArrayLength( jo_args )) );
    for ( sal_Int32 nPos = 0; nPos < nParams; ++nPos )
    {
        typelib_MethodParameter const & param = pParams[ nPos ];
        typelib_TypeDescriptionReference * type = param.pTypeRef;
        
        uno_args[ nPos ] = &uno_args_mem[ nPos ];
        if (typelib_TypeClass_STRUCT == type->eTypeClass ||
            typelib_TypeClass_EXCEPTION == type->eTypeClass)
        {
            TypeDescr td( type );
            if (td.get()->nSize > sizeof (largest))
                uno_args[ nPos ] = alloca( td.get()->nSize );
        }
        
        if (param.bIn)
        {
            JLocalAutoRef jo_arg( attach, attach->GetObjectArrayElement( jo_args, nPos ) );
            attach.ensure_no_exception();
            jvalue java_arg;
            java_arg.l = jo_arg.get();
            try
            {
                map_to_uno(
                    attach, uno_args[ nPos ], java_arg, type, 0,
                    false /* no assign */, sal_False != param.bOut,
                    true /* special wrapped integral types */ );
            }
            catch (...)
            {
                // cleanup uno in args
                for ( sal_Int32 n = 0; n < nPos; ++n )
                {
                    typelib_MethodParameter const & param = pParams[ nPos ];
                    if (param.bIn)
                    {
                        uno_type_destructData( uno_args[ nPos ], param.pTypeRef, 0 );
                    }
                }
                throw;
            }
        }
    }
    
    uno_Any uno_exc_holder;
    uno_Any * uno_exc = &uno_exc_holder;
    // call binary uno
    (*pUnoI->pDispatcher)( pUnoI, member_td, uno_ret, uno_args, &uno_exc );
    
    if (0 == uno_exc)
    {
        // convert out args; destruct uno args
        for ( sal_Int32 nPos = 0; nPos < nParams; ++nPos )
        {
            typelib_MethodParameter const & param = pParams[ nPos ];
            if (param.bOut)
            {
                try
                {
                    jvalue java_arg;
                    map_to_java(
                        attach, &java_arg, uno_args[ nPos ], param.pTypeRef, 0, true, true );
                    JLocalAutoRef jo_arg( attach, java_arg.l );
                    attach->SetObjectArrayElement( jo_args, nPos, jo_arg.get() );
                    attach.ensure_no_exception();
                }
                catch (...)
                {
                    // cleanup further uno args
                    for ( sal_Int32 n = nPos; n < nParams; ++n )
                    {
                        uno_type_destructData( uno_args[ nPos ], pParams[ nPos ].pTypeRef, 0 );
                    }
                    // cleanup uno return value
                    if (return_type)
                    {
                        uno_type_destructData( uno_ret, return_type, 0 );
                    }
                    throw;
                }
            }
            uno_type_destructData( uno_args[ nPos ], param.pTypeRef, 0 );
        }
        
        if ((0 != return_type) &&
            (typelib_TypeClass_VOID != return_type->eTypeClass))
        {
            // convert uno return value
            try
            {
                jvalue java_ret;
                map_to_java(
                    attach, &java_ret, uno_ret, return_type, 0, true, false,
                    true /* special_wrapped_integral_types */ );
                return java_ret.l;
            }
            catch (...)
            {
                uno_type_destructData( uno_ret, return_type, 0 );
                throw;
            }
        }
        return 0; // void return
    }
    else // exception occured
    {
        // destruct uno in args
        for ( sal_Int32 nPos = 0; nPos < nParams; ++nPos )
        {
            typelib_MethodParameter const & param = pParams[ nPos ];
            if (param.bIn)
            {
                uno_type_destructData( uno_args[ nPos ], param.pTypeRef, 0 );
            }
        }
        
        handle_uno_exc( attach, uno_exc );
        return 0;
    }
}

}

using namespace ::jni_bridge;

extern "C"
{

//##################################################################################################
JNIEXPORT jobject JNICALL Java_com_sun_star_bridges_jni_1uno_JNI_1proxy_dispatch_1call(
    JNIEnv * jni_env, jobject jo_proxy, jlong bridge_handle,
    jstring jo_decl_class, jstring jo_method, jobjectArray jo_args /* may be 0 */ )
    SAL_THROW_EXTERN_C()
{
    jni_Bridge const * bridge = reinterpret_cast< jni_Bridge const * >( bridge_handle );
    JNI_info const * jni_info = bridge->m_jni_info;
    JNI_attach attach( bridge->m_java_env, jni_env );
    
    try
    {
        OUString method_name( jstring_to_oustring( attach, jo_method ) );
#ifdef DEBUG
        OString cstr_msg(
            OUStringToOString( 
                OUSTR("java dispatch call occurs: ") + method_name, RTL_TEXTENCODING_ASCII_US ) );
        OSL_TRACE( cstr_msg.getStr() );
#endif
        
        // special IQueryInterface.queryInterface()
        if (method_name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("queryInterface") ))
        {
            // oid
            JLocalAutoRef jo_oid(
                attach, attach->GetObjectField( jo_proxy, jni_info->m_field_JNI_proxy_m_oid ) );
            // type
            JLocalAutoRef jo_type( attach, attach->GetObjectArrayElement( jo_args, 0 ) );
            attach.ensure_no_exception();
            
            JLocalAutoRef jo_type_name(
                attach, attach->GetObjectField(
                    jo_type.get(), jni_info->m_field_Type__typeName ) );
            if (! jo_type_name.is())
                throw BridgeRuntimeError( OUSTR("incomplete type object: no type name!") );
            OUString type_name( jstring_to_oustring( attach, (jstring)jo_type_name.get() ) );
            JNI_type_info const * info = jni_info->get_type_info( attach, type_name );
            
            // getRegisteredInterface() already tested in JNI_proxy
            // perform queryInterface call on binary uno interface
            uno_Interface * pUnoI = reinterpret_cast< uno_Interface * >(
                attach->GetLongField(
                    jo_proxy, jni_info->m_field_JNI_proxy_m_receiver_handle ) );
            
            uno_Any uno_ret;
            void * uno_args[] = { &info->m_td.get()->pWeakRef };
            uno_Any * uno_exc;
            // call binary uno
            (*pUnoI->pDispatcher)(
                pUnoI, jni_info->m_XInterface_queryInterface_td.get(),
                &uno_ret, uno_args, &uno_exc );
            if (0 == uno_exc)
            {
                jobject jo_ret = 0;
                if (typelib_TypeClass_INTERFACE == uno_ret.pType->eTypeClass)
                {
                    uno_Interface * pUnoRet = (uno_Interface *)uno_ret.pReserved;
                    if (0 != pUnoRet)
                    {
                        try
                        {
                            jo_ret = bridge->map_uno2java( attach, pUnoRet, info );
                        }
                        catch (...)
                        {
                            uno_any_destruct( &uno_ret, 0 );
                            throw;
                        }
                    }
                }
                uno_any_destruct( &uno_ret, 0 );
                return jo_ret;
            }
            else
            {
                bridge->handle_uno_exc( attach, uno_exc );
                return 0;
            }
        }
        
        // determine exact interface td
        typelib_InterfaceTypeDescription * td =
            reinterpret_cast< typelib_InterfaceTypeDescription * >(
                attach->GetLongField( jo_proxy, jni_info->m_field_JNI_proxy_m_td_handle ) );
        OUString iface_name( jstring_to_oustring( attach, jo_decl_class ) );
        while (! reinterpret_cast< OUString const * >(
                   &((typelib_TypeDescription *)td)->pTypeName )->equals( iface_name ))
        {
            td = td->pBaseTypeDescription;
        }
        
        uno_Interface * pUnoI =
            reinterpret_cast< uno_Interface * >(
                attach->GetLongField( jo_proxy, jni_info->m_field_JNI_proxy_m_receiver_handle ) );
        
        typelib_TypeDescriptionReference ** ppMembers = td->ppMembers;
        for ( sal_Int32 nPos = td->nMembers; nPos--; )
        {
            // try to avoid getting typedescription as long as possible,
            // because of a Mutex.acquire() in typelib_typedescriptionreference_getDescription()
            typelib_TypeDescriptionReference * member_type = ppMembers[ nPos ];
            
            // check method_name against fully qualified type_name of member_type
            OUString const & type_name =
                *reinterpret_cast< OUString const * >( &member_type->pTypeName );
            if (typelib_TypeClass_INTERFACE_METHOD == member_type->eTypeClass)
            {
                if ((method_name.getLength() < type_name.getLength()) &&
                    (':' == type_name[ type_name.getLength() - method_name.getLength() -1 ]) &&
                    (0 == rtl_ustr_compare(
                        method_name.getStr(),
                        type_name.getStr() + type_name.getLength() - method_name.getLength() )
                     /* ends with method_name: */))
                {
                    TypeDescr member_td( ppMembers[ nPos ] );
                    typelib_InterfaceMethodTypeDescription * method_td =
                        (typelib_InterfaceMethodTypeDescription *)member_td.get();
                    return bridge->call_uno(
                        attach, pUnoI, member_td.get(),
                        method_td->pReturnTypeRef, method_td->nParams, method_td->pParams,
                        jo_args );
                }
            }
            else // attribute
            {
                OSL_ASSERT( typelib_TypeClass_INTERFACE_ATTRIBUTE == member_type->eTypeClass );
                
                if ((method_name.getLength() > 3) &&
                    (method_name.getLength() < (type_name.getLength() +3)) &&
                    (':' == type_name[ type_name.getLength() - method_name.getLength() -1 +3 ]) &&
                    ('e' == method_name[ 1 ]) && ('t' == method_name[ 2 ]) &&
                    (0 == rtl_ustr_compare(
                        method_name.getStr() +3,
                        type_name.getStr() + type_name.getLength() - method_name.getLength() +3 )
                     /* ends with method_name.copy(3): */))
                {
                    if ('g' == method_name[ 0 ])
                    {
                        TypeDescr member_td( ppMembers[ nPos ] );
                        typelib_InterfaceAttributeTypeDescription * attribute_td =
                            (typelib_InterfaceAttributeTypeDescription *)member_td.get();
                        return bridge->call_uno(
                            attach, pUnoI, member_td.get(),
                            attribute_td->pAttributeTypeRef, 0, 0,
                            jo_args );
                    }
                    else if ('s' == method_name[ 0 ])
                    {
                        TypeDescr member_td( ppMembers[ nPos ] );
                        typelib_InterfaceAttributeTypeDescription * attribute_td =
                            (typelib_InterfaceAttributeTypeDescription *)member_td.get();
                        if (! attribute_td->bReadOnly)
                        {
                            typelib_MethodParameter param;
                            param.pTypeRef = attribute_td->pAttributeTypeRef;
                            param.bIn = sal_True;
                            param.bOut = sal_False;
                            return bridge->call_uno(
                                attach, pUnoI, member_td.get(),
                                0 /* indicates void return */, 1, &param,
                                jo_args );
                        }
                    }
                }
            }
        }
        // the thing that should not be... no method info found!
        throw BridgeRuntimeError( OUSTR("no method info found for call: ") + method_name );
    }
    catch (BridgeRuntimeError & err)
    {
        // notify RuntimeException
        OString cstr_msg(
            OUStringToOString(
                OUSTR("[jni_uno bridge error] ") + err.m_message, RTL_TEXTENCODING_ASCII_US ) );
        OSL_TRACE( cstr_msg.getStr() );
        jint res = attach->ThrowNew( jni_info->m_class_RuntimeException, cstr_msg.getStr() );
        OSL_ASSERT( 0 == res );
        return 0;
    }
}
//##################################################################################################
JNIEXPORT void JNICALL Java_com_sun_star_bridges_jni_1uno_JNI_1proxy_finalize__J(
    JNIEnv * jni_env, jobject jo_proxy, jlong bridge_handle )
    SAL_THROW_EXTERN_C()
{
    jni_Bridge const * bridge = reinterpret_cast< jni_Bridge const * >( bridge_handle );
    JNI_info const * jni_info = bridge->m_jni_info;
    JNI_attach attach( bridge->m_java_env, jni_env );
    
    uno_Interface * pUnoI = reinterpret_cast< uno_Interface * >(
        attach->GetLongField( jo_proxy, jni_info->m_field_JNI_proxy_m_receiver_handle ) );
    typelib_TypeDescription * td = reinterpret_cast< typelib_TypeDescription * >(
        attach->GetLongField( jo_proxy, jni_info->m_field_JNI_proxy_m_td_handle ) );
    
#ifdef DEBUG
    JLocalAutoRef jo_oid(
        attach, attach->GetObjectField( jo_proxy, jni_info->m_field_JNI_proxy_m_oid ) );
    OUString oid( jstring_to_oustring( attach, (jstring)jo_oid.get() ) );
    OString cstr_msg(
        OUStringToOString( 
            OUSTR("freeing java uno proxy: ") + oid, RTL_TEXTENCODING_ASCII_US ) );
    OSL_TRACE( cstr_msg.getStr() );
#endif
    // revoke from uno env; has already been revoked from java env
    (*bridge->m_uno_env->revokeInterface)( bridge->m_uno_env, pUnoI );
    // release receiver
    (*pUnoI->release)( pUnoI );
    // release typedescription handle
    typelib_typedescription_release( td );
    // release bridge handle
    bridge->release();
}

}
