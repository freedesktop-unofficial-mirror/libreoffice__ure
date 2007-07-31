/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: registry.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2007-07-31 14:01:44 $
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

#ifndef _CODEMAKER_REGISTRY_HXX_
#define _CODEMAKER_REGISTRY_HXX_

#ifndef _RTL_ALLOC_H_
#include <rtl/alloc.h>
#endif

#ifndef _OSL_INTERLOCK_H_
#include <osl/interlck.h>
#endif

#ifndef _REGISTRY_REGISTRY_HXX_
#include	<registry/registry.hxx>
#endif
#include "registry/reader.hxx"
#include "registry/version.h"

#ifndef _CODEMAKER_OPTIONS_HXX_
#include	<codemaker/options.hxx>
#endif

struct TypeReader_Impl
{
    TypeReader_Impl(const sal_uInt8* buffer, 
                       sal_uInt32 bufferLen, 
                       sal_Bool copyData)
        : m_refCount(0)
        , m_copyData(copyData)
        , m_blopSize(bufferLen)
        , m_pBlop(buffer)
        {
            if (copyData)
            {
                m_pBlop = (sal_uInt8*)rtl_allocateMemory(bufferLen);
                rtl_copyMemory((void*)m_pBlop, buffer, bufferLen);
            } else
            {
                m_blopSize = bufferLen;
                m_pBlop = buffer;
            }

            m_pReader = new typereg::Reader(
                m_pBlop, m_blopSize, false, TYPEREG_VERSION_1);
        }

    ~TypeReader_Impl()
        {
            if (m_copyData && m_pReader)
            {
                delete m_pReader;
            }
        }

    sal_Int32			m_refCount;
    sal_Bool			m_copyData;
    sal_Int32			m_blopSize;
    const sal_uInt8* 	m_pBlop;
    typereg::Reader*	m_pReader;
};	

class TypeReader
{
/*
    inline TypeReader(const RegistryTypeReader_Api* pApi,
                              const sal_uInt8* buffer, 
                              sal_uInt32 bufferLen, 
                              sal_Bool copyData);
*/
public:
    inline TypeReader()
        : m_pImpl(NULL)
    {}

    inline TypeReader(        const sal_uInt8* buffer, 
                              sal_uInt32 bufferLen, 
                              sal_Bool copyData)
    {
        m_pImpl = new TypeReader_Impl(buffer, bufferLen, copyData);
        acquire();
    }

    inline TypeReader(const TypeReader& toCopy)
        : m_pImpl(toCopy.m_pImpl)
    { 
        acquire();
    }

    inline ~TypeReader()
    { 
        release();
    }    

    inline void acquire()
    { 
        if (m_pImpl)
            osl_incrementInterlockedCount(&m_pImpl->m_refCount);
    }

    inline void release()
    { 
        if (m_pImpl && 0 == osl_decrementInterlockedCount(&m_pImpl->m_refCount))
        {
            delete m_pImpl;
        }
    }

    inline TypeReader& operator = ( const TypeReader& value )
    {
        release();
        m_pImpl = value.m_pImpl;
        acquire();
        return *this;
    }

    inline sal_Bool			isValid() const 
        { 
            if (m_pImpl)
                return m_pImpl->m_pReader->isValid();
            else
                return sal_False;
        }

    inline RTTypeClass 				getTypeClass() const
        { return m_pImpl->m_pReader->getTypeClass(); }
    inline const ::rtl::OString 	getTypeName() const
        { return inGlobalSet( m_pImpl->m_pReader->getTypeName() ); }
    inline sal_uInt16 getSuperTypeCount() const
        { return m_pImpl->m_pReader->getSuperTypeCount(); }
    inline const ::rtl::OString 	getSuperTypeName(sal_uInt16 index) const
        { return inGlobalSet( m_pImpl->m_pReader->getSuperTypeName(index) ); }
    inline const ::rtl::OString 	getDoku() const
        { return inGlobalSet( m_pImpl->m_pReader->getDocumentation() ); }
    inline const ::rtl::OString 	getFileName() const
        { return inGlobalSet( m_pImpl->m_pReader->getFileName() ); }
    inline sal_uInt32				getFieldCount() const
        { return m_pImpl->m_pReader->getFieldCount(); }
    inline const ::rtl::OString 	getFieldName( sal_uInt16 index ) const
        { return inGlobalSet( m_pImpl->m_pReader->getFieldName(index) ); }
    inline const ::rtl::OString		getFieldType( sal_uInt16 index ) const
        { return inGlobalSet( m_pImpl->m_pReader->getFieldTypeName(index) ); }
    inline RTFieldAccess			getFieldAccess( sal_uInt16 index ) const
        { return m_pImpl->m_pReader->getFieldFlags(index); }
    inline RTConstValue				getFieldConstValue( sal_uInt16 index ) const
        { return m_pImpl->m_pReader->getFieldValue(index); }
    inline const ::rtl::OString 	getFieldDoku( sal_uInt16 index ) const
        { return inGlobalSet( m_pImpl->m_pReader->getFieldDocumentation(index) ); }
    inline const ::rtl::OString 	getFieldFileName( sal_uInt16 index ) const
        { return inGlobalSet( m_pImpl->m_pReader->getFieldFileName(index) ); }
    inline sal_uInt32				getMethodCount() const
        { return m_pImpl->m_pReader->getMethodCount(); }
    inline const ::rtl::OString 	getMethodName( sal_uInt16 index ) const
        { return inGlobalSet( m_pImpl->m_pReader->getMethodName(index) ); }
    inline sal_uInt32    			getMethodParamCount( sal_uInt16 index ) const
        { return m_pImpl->m_pReader->getMethodParameterCount(index); }
    inline const ::rtl::OString 	getMethodParamType( sal_uInt16 index, sal_uInt16 paramIndex ) const
        { return inGlobalSet( m_pImpl->m_pReader->getMethodParameterTypeName(index,paramIndex) ); }
    inline const ::rtl::OString 	getMethodParamName( sal_uInt16 index, sal_uInt16 paramIndex ) const
        { return inGlobalSet( m_pImpl->m_pReader->getMethodParameterName(index,paramIndex) ); }
    inline RTParamMode				getMethodParamMode( sal_uInt16 index, sal_uInt16 paramIndex ) const
        { return m_pImpl->m_pReader->getMethodParameterFlags(index,paramIndex); }
    inline sal_uInt32    			getMethodExcCount( sal_uInt16 index ) const
        { return m_pImpl->m_pReader->getMethodExceptionCount(index); }
    inline const ::rtl::OString 	getMethodExcType( sal_uInt16 index, sal_uInt16 excIndex ) const
        { return inGlobalSet( m_pImpl->m_pReader->getMethodExceptionTypeName(index,excIndex) ); }
    inline const ::rtl::OString 	getMethodReturnType( sal_uInt16 index ) const
        { return inGlobalSet( m_pImpl->m_pReader->getMethodReturnTypeName(index) ); }
    inline RTMethodMode				getMethodMode( sal_uInt16 index ) const
        { return m_pImpl->m_pReader->getMethodFlags(index); }
    inline const ::rtl::OString 	getMethodDoku( sal_uInt16 index ) const
        { return inGlobalSet( m_pImpl->m_pReader->getMethodDocumentation(index) ); }

    inline sal_uInt32				getReferenceCount() const
        { return m_pImpl->m_pReader->getReferenceCount(); }
    inline const ::rtl::OString 	getReferenceName( sal_uInt16 index ) const
        { return inGlobalSet( m_pImpl->m_pReader->getReferenceTypeName(index) ); }
    inline RTReferenceType			getReferenceType( sal_uInt16 index ) const
        { return m_pImpl->m_pReader->getReferenceSort(index); }
    inline const ::rtl::OString 	getReferenceDoku( sal_uInt16 index ) const
        { return inGlobalSet( m_pImpl->m_pReader->getReferenceDocumentation(index) ); }

    inline sal_uInt32 getBlopSize() const
        { return m_pImpl->m_blopSize; }

    inline const sal_uInt8* getBlop() const
        { return m_pImpl->m_pBlop; }

private:
    TypeReader_Impl* m_pImpl;
};		

    
#endif // _CODEMAKER_REGISTRY_HXX_
