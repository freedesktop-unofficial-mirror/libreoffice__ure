/*************************************************************************
 *
 *  $RCSfile: tdiface.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: kz $ $Date: 2004-03-25 14:48:43 $
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

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

#include <com/sun/star/reflection/XInterfaceMemberTypeDescription.hpp>
#include <com/sun/star/reflection/XInterfaceAttributeTypeDescription.hpp>
#include <com/sun/star/reflection/XInterfaceMethodTypeDescription.hpp>
#include <com/sun/star/reflection/XMethodParameter.hpp>

#ifndef _STOC_RDBTDP_BASE_HXX
#include "base.hxx"
#endif

#include <set>

namespace stoc_rdbtdp
{

//==================================================================================================
class MethodParameterImpl : public WeakImplHelper1< XMethodParameter >
{
    Reference< XHierarchicalNameAccess > _xTDMgr;
    
    OUString						_aName;
    OUString						_aTypeName;
    Reference< XTypeDescription >	_xType;
    
    sal_Bool						_bIn;
    sal_Bool						_bOut;
    sal_Int32						_nPosition;
    
public:
    MethodParameterImpl( const Reference< XHierarchicalNameAccess > & xTDMgr,
                         const OUString & rParamName, const OUString & rParamType,
                         sal_Bool bIn, sal_Bool bOut, sal_Int32 nPosition )
        : _xTDMgr( xTDMgr )
        , _aName( rParamName )
        , _aTypeName( rParamType )
        , _bIn( bIn )
        , _bOut( bOut )
        , _nPosition( nPosition )
        {
            g_moduleCount.modCnt.acquire( &g_moduleCount.modCnt );
        }
    virtual ~MethodParameterImpl();
    
    // XMethodParameter
    virtual OUString SAL_CALL getName() throw(::com::sun::star::uno::RuntimeException);
    virtual Reference< XTypeDescription > SAL_CALL getType() throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL isIn() throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL isOut() throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getPosition() throw(::com::sun::star::uno::RuntimeException);
};

MethodParameterImpl::~MethodParameterImpl()
{
    g_moduleCount.modCnt.release( &g_moduleCount.modCnt );
}
// XMethodParameter
//__________________________________________________________________________________________________
OUString MethodParameterImpl::getName()
    throw(::com::sun::star::uno::RuntimeException)
{
    return _aName;
}
//__________________________________________________________________________________________________
Reference<XTypeDescription > MethodParameterImpl::getType()
    throw(::com::sun::star::uno::RuntimeException)
{
    if (!_xType.is() && _aTypeName.getLength())
    {
        try
        {
            Reference< XTypeDescription > xType;
            if (_xTDMgr->getByHierarchicalName( _aTypeName ) >>= xType)
            {
                MutexGuard aGuard( getMutex() );
                if (! _xType.is())
                    _xType = xType;
                return _xType;
            }
        }
        catch (NoSuchElementException &)
        {
        }
        // never try again, if no td was found
        _aTypeName = OUString();
    }
    return _xType;
}
//__________________________________________________________________________________________________
sal_Bool MethodParameterImpl::isIn()
    throw(::com::sun::star::uno::RuntimeException)
{
    return _bIn;
}
//__________________________________________________________________________________________________
sal_Bool MethodParameterImpl::isOut()
    throw(::com::sun::star::uno::RuntimeException)
{
    return _bOut;
}
//__________________________________________________________________________________________________
sal_Int32 MethodParameterImpl::getPosition()
    throw(::com::sun::star::uno::RuntimeException)
{
    return _nPosition;
}


//##################################################################################################
//##################################################################################################
//##################################################################################################


//==================================================================================================
class InterfaceMethodImpl : public WeakImplHelper1< XInterfaceMethodTypeDescription >
{
    Reference< XHierarchicalNameAccess >  _xTDMgr;
    
    OUString							  _aTypeName;
    OUString							  _aMemberName;
    
    OUString							  _aReturnType;
    Reference< XTypeDescription >		  _xReturnTD;
    
    Sequence< sal_Int8 >				  _aBytes;
    sal_uInt16							  _nMethodIndex;
    Sequence< Reference< XMethodParameter > > * _pParams;
    Sequence< Reference< XTypeDescription > > * _pExceptions;
    
    sal_Bool							  _bIsOneWay;
    sal_Int32							  _nPosition;
    
public:
    InterfaceMethodImpl( const Reference< XHierarchicalNameAccess > & xTDMgr,
                         const OUString & rTypeName,
                         const OUString & rMemberName,
                         const OUString & rReturnType,
                         const Sequence< sal_Int8 > & rBytes,
                         sal_uInt16 nMethodIndex,
                         sal_Bool bIsOneWay,
                         sal_Int32 nPosition )
        : _xTDMgr( xTDMgr )
        , _aTypeName( rTypeName )
        , _aMemberName( rMemberName )
        , _aReturnType( rReturnType )
        , _aBytes( rBytes )
        , _nMethodIndex( nMethodIndex )
        , _pParams( 0 )
        , _pExceptions( 0 )
        , _bIsOneWay( bIsOneWay )
        , _nPosition( nPosition )
        {
            g_moduleCount.modCnt.acquire( &g_moduleCount.modCnt );
        }
    virtual ~InterfaceMethodImpl();
    
    // XTypeDescription
    virtual TypeClass SAL_CALL getTypeClass() throw(::com::sun::star::uno::RuntimeException);
    virtual OUString SAL_CALL getName() throw(::com::sun::star::uno::RuntimeException);
    
    // XInterfaceMemberTypeDescription
    virtual OUString SAL_CALL getMemberName() throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getPosition() throw(::com::sun::star::uno::RuntimeException);

    // XInterfaceMethodTypeDescription
    virtual Reference< XTypeDescription > SAL_CALL getReturnType() throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL isOneway() throw(::com::sun::star::uno::RuntimeException);
    virtual Sequence< Reference< XMethodParameter > > SAL_CALL getParameters() throw(::com::sun::star::uno::RuntimeException);
    virtual Sequence< Reference< XTypeDescription > > SAL_CALL getExceptions() throw(::com::sun::star::uno::RuntimeException);
};
//__________________________________________________________________________________________________
InterfaceMethodImpl::~InterfaceMethodImpl()
{
    delete _pParams;
    delete _pExceptions;
    g_moduleCount.modCnt.release( &g_moduleCount.modCnt );
}

// XTypeDescription
//__________________________________________________________________________________________________
TypeClass InterfaceMethodImpl::getTypeClass()
    throw(::com::sun::star::uno::RuntimeException)
{
    return TypeClass_INTERFACE_METHOD;
}
//__________________________________________________________________________________________________
OUString InterfaceMethodImpl::getName()
    throw(::com::sun::star::uno::RuntimeException)
{
    return _aTypeName;
}

// XInterfaceMemberTypeDescription
//__________________________________________________________________________________________________
OUString InterfaceMethodImpl::getMemberName()
    throw(::com::sun::star::uno::RuntimeException)
{
    return _aMemberName;
}
//__________________________________________________________________________________________________
sal_Int32 InterfaceMethodImpl::getPosition()
    throw(::com::sun::star::uno::RuntimeException)
{
    return _nPosition;
}

// XInterfaceMethodTypeDescription
//__________________________________________________________________________________________________
Reference<XTypeDescription > InterfaceMethodImpl::getReturnType()
    throw(::com::sun::star::uno::RuntimeException)
{
    if (!_xReturnTD.is() && _aReturnType.getLength())
    {
        try
        {
            Reference< XTypeDescription > xReturnTD;
            if (_xTDMgr->getByHierarchicalName( _aReturnType ) >>= xReturnTD)
            {
                MutexGuard aGuard( getMutex() );
                if (! _xReturnTD.is())
                    _xReturnTD = xReturnTD;
                return _xReturnTD;
            }
        }
        catch (NoSuchElementException &)
        {
        }
        // never try again, if no td was found
        _aReturnType = OUString();
    }
    return _xReturnTD;
}
//__________________________________________________________________________________________________
sal_Bool InterfaceMethodImpl::isOneway()
    throw(::com::sun::star::uno::RuntimeException)
{
    return _bIsOneWay;
}
//__________________________________________________________________________________________________
Sequence<Reference<XMethodParameter > > InterfaceMethodImpl::getParameters()
    throw(::com::sun::star::uno::RuntimeException)
{
    if (! _pParams)
    {
        RegistryTypeReaderLoader aLoader;
        RegistryTypeReader aReader(
            aLoader, (const sal_uInt8 *)_aBytes.getConstArray(),
            _aBytes.getLength(), sal_False );
        
        sal_uInt16 nParams = (sal_uInt16)aReader.getMethodParamCount( _nMethodIndex );
        Sequence< Reference< XMethodParameter > > * pTempParams =
            new Sequence< Reference< XMethodParameter > >( nParams );
        Reference< XMethodParameter > * pParams = pTempParams->getArray();
        
        while (nParams--)
        {
            RTParamMode eMode = aReader.getMethodParamMode( _nMethodIndex, nParams );
            
            pParams[nParams] = new MethodParameterImpl(
                _xTDMgr,
                aReader.getMethodParamName( _nMethodIndex, nParams ),
                aReader.getMethodParamType( _nMethodIndex, nParams ).replace( '/', '.' ),
                (eMode == RT_PARAM_IN || eMode == RT_PARAM_INOUT),
                (eMode == RT_PARAM_OUT || eMode == RT_PARAM_INOUT),
                nParams );
        }
        
        ClearableMutexGuard aGuard( getMutex() );
        if (_pParams)
        {
            aGuard.clear();
            delete pTempParams;
        }
        else
        {
            _pParams = pTempParams;
        }
    }
    return *_pParams;
}
//__________________________________________________________________________________________________
Sequence<Reference<XTypeDescription > > InterfaceMethodImpl::getExceptions()
    throw(::com::sun::star::uno::RuntimeException)
{
    if (! _pExceptions)
    {
        RegistryTypeReaderLoader aLoader;
        RegistryTypeReader aReader(
            aLoader, (const sal_uInt8 *)_aBytes.getConstArray(),
            _aBytes.getLength(), sal_False );
        
        sal_uInt16 nExc = (sal_uInt16)aReader.getMethodExcCount( _nMethodIndex );
        Sequence< Reference< XTypeDescription > > * pExceptions =
            new Sequence< Reference< XTypeDescription > >( nExc );
        Reference< XTypeDescription > * pExc = pExceptions->getArray();
        
        while (nExc--)
        {
            try
            {
                OUString aMethodExcName( aReader.getMethodExcType( _nMethodIndex, nExc ) );
                _xTDMgr->getByHierarchicalName( aMethodExcName.replace( '/', '.' ) )
                    >>= pExc[ nExc ];
            }
            catch (NoSuchElementException &)
            {
            }
            OSL_ENSURE( pExc[nExc].is(), "### exception type unknown!" );
        }
        
        ClearableMutexGuard aGuard( getMutex() );
        if (_pExceptions)
        {
            aGuard.clear();
            delete pExceptions;
        }
        else
        {
            _pExceptions = pExceptions;
        }
    }
    return *_pExceptions;
}


//##################################################################################################
//##################################################################################################
//##################################################################################################


//==================================================================================================
class InterfaceAttributeImpl : public WeakImplHelper1< XInterfaceAttributeTypeDescription >
{
    Reference< XHierarchicalNameAccess >  _xTDMgr;
    
    OUString							  _aTypeName;
    OUString							  _aMemberName;
    
    OUString							  _aMemberTypeName;
    Reference< XTypeDescription >		  _xMemberTD;
    
    sal_Bool							  _bReadOnly;
    sal_Int32							  _nPosition;
    
public:
    InterfaceAttributeImpl( const Reference< XHierarchicalNameAccess > & xTDMgr,
                            const OUString & rTypeName,
                            const OUString & rMemberName,
                            const OUString & rMemberTypeName,
                            sal_Bool bReadOnly,
                            sal_Int32 nPosition )
        : _xTDMgr( xTDMgr )
        , _aTypeName( rTypeName )
        , _aMemberName( rMemberName )
        , _aMemberTypeName( rMemberTypeName )
        , _bReadOnly( bReadOnly )
        , _nPosition( nPosition )
        {
            g_moduleCount.modCnt.acquire( &g_moduleCount.modCnt );
        }
    virtual ~InterfaceAttributeImpl();
    
    // XTypeDescription
    virtual TypeClass SAL_CALL getTypeClass() throw(::com::sun::star::uno::RuntimeException);
    virtual OUString SAL_CALL getName() throw(::com::sun::star::uno::RuntimeException);
    
    // XInterfaceMemberTypeDescription
    virtual OUString SAL_CALL getMemberName() throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getPosition() throw(::com::sun::star::uno::RuntimeException);
    
    // XInterfaceAttributeTypeDescription
    virtual sal_Bool SAL_CALL isReadOnly() throw(::com::sun::star::uno::RuntimeException);
    virtual Reference< XTypeDescription > SAL_CALL getType() throw(::com::sun::star::uno::RuntimeException);
};

InterfaceAttributeImpl::~InterfaceAttributeImpl()
{
    g_moduleCount.modCnt.release( &g_moduleCount.modCnt );
}
// XTypeDescription
//__________________________________________________________________________________________________
TypeClass InterfaceAttributeImpl::getTypeClass()
    throw(::com::sun::star::uno::RuntimeException)
{
    return TypeClass_INTERFACE_ATTRIBUTE;
}
//__________________________________________________________________________________________________
OUString InterfaceAttributeImpl::getName()
    throw(::com::sun::star::uno::RuntimeException)
{
    return _aTypeName;
}

// XInterfaceMemberTypeDescription
//__________________________________________________________________________________________________
OUString InterfaceAttributeImpl::getMemberName()
    throw(::com::sun::star::uno::RuntimeException)
{
    return _aMemberName;
}
//__________________________________________________________________________________________________
sal_Int32 InterfaceAttributeImpl::getPosition()
    throw(::com::sun::star::uno::RuntimeException)
{
    return _nPosition;
}

// XInterfaceAttributeTypeDescription
//__________________________________________________________________________________________________
sal_Bool InterfaceAttributeImpl::isReadOnly()
    throw(::com::sun::star::uno::RuntimeException)
{
    return _bReadOnly;
}
//__________________________________________________________________________________________________
Reference<XTypeDescription > InterfaceAttributeImpl::getType()
    throw(::com::sun::star::uno::RuntimeException)
{
    if (!_xMemberTD.is() && _aMemberTypeName.getLength())
    {
        try
        {
            Reference< XTypeDescription > xMemberTD;
            if (_xTDMgr->getByHierarchicalName( _aMemberTypeName ) >>= xMemberTD)
            {
                MutexGuard aGuard( getMutex() );
                if (! _xMemberTD.is())
                    _xMemberTD = xMemberTD;
                return _xMemberTD;
            }
        }
        catch (NoSuchElementException &)
        {
        }
        // never try again, if no td was found
        _aMemberTypeName = OUString();
    }
    return _xMemberTD;
}


//##################################################################################################
//##################################################################################################
//##################################################################################################

namespace {

class BaseOffset {
public:
    BaseOffset(Reference< XInterfaceTypeDescription2 > const & desc);

    sal_Int32 get() const { return offset; }

private:
    void calculateBases(Reference< XInterfaceTypeDescription2 > const & desc);

    void calculate(Reference< XInterfaceTypeDescription2 > const & desc);

    std::set< rtl::OUString > set;
    sal_Int32 offset;
};

BaseOffset::BaseOffset(Reference< XInterfaceTypeDescription2 > const & desc) {
    offset = 0;
    calculateBases(desc);
}

void BaseOffset::calculateBases(
    Reference< XInterfaceTypeDescription2 > const & desc)
{
    Sequence< Reference < XInterfaceTypeDescription2 > > bases(
        desc->getBaseTypes());
    for (sal_Int32 i = 0; i < bases.getLength(); ++i) {
        calculate(bases[i]);
    }
}

void BaseOffset::calculate(Reference< XInterfaceTypeDescription2 > const & desc)
{
    if (set.insert(desc->getName()).second) {
        calculateBases(desc);
        offset += desc->getMembers().getLength();
    }
}

}

//__________________________________________________________________________________________________
InterfaceTypeDescriptionImpl::InterfaceTypeDescriptionImpl(
    const Reference< XHierarchicalNameAccess > & xTDMgr,
    const OUString & rName, const Sequence< OUString > & rBaseTypes,
    const RTUik & rUik, const Sequence< sal_Int8 > & rBytes )
    : _xTDMgr( xTDMgr )
    , _aName( rName )
    , _aBaseTypes( rBaseTypes )
    , _aBytes( rBytes )
    , _pAttributes( 0 )
    , _pMethods( 0 )
{
    // uik
    g_moduleCount.modCnt.acquire( &g_moduleCount.modCnt );
    _aUik.m_Data1 = rUik.m_Data1;
    _aUik.m_Data2 = rUik.m_Data2;
    _aUik.m_Data3 = rUik.m_Data3;
    _aUik.m_Data4 = rUik.m_Data4;
    _aUik.m_Data5 = rUik.m_Data5;
}
//__________________________________________________________________________________________________
InterfaceTypeDescriptionImpl::~InterfaceTypeDescriptionImpl()
{
    delete _pAttributes;
    delete _pMethods;
    g_moduleCount.modCnt.release( &g_moduleCount.modCnt );
}

// XTypeDescription
//__________________________________________________________________________________________________
TypeClass InterfaceTypeDescriptionImpl::getTypeClass()
    throw(::com::sun::star::uno::RuntimeException)
{
    return TypeClass_INTERFACE;
}
//__________________________________________________________________________________________________
OUString InterfaceTypeDescriptionImpl::getName()
    throw(::com::sun::star::uno::RuntimeException)
{
    return _aName;
}

// XInterfaceTypeDescription2
//__________________________________________________________________________________________________
Reference< XTypeDescription > InterfaceTypeDescriptionImpl::getBaseType()
    throw(::com::sun::star::uno::RuntimeException)
{
    Sequence< Reference< XInterfaceTypeDescription2 > > aBaseTypes(
        getBaseTypes());
    return aBaseTypes.getLength() >= 1
        ? Reference< XTypeDescription >(aBaseTypes[0], UNO_QUERY) : 0;
}
//__________________________________________________________________________________________________
Uik SAL_CALL InterfaceTypeDescriptionImpl::getUik()
    throw(::com::sun::star::uno::RuntimeException)
{
    return _aUik;
}
//__________________________________________________________________________________________________
Sequence< Reference< XInterfaceMemberTypeDescription > > InterfaceTypeDescriptionImpl::getMembers()
    throw(::com::sun::star::uno::RuntimeException)
{
    if (! _pMethods)
    {
        RegistryTypeReaderLoader aLoader;
        RegistryTypeReader aReader(
            aLoader, (const sal_uInt8 *)_aBytes.getConstArray(),
            _aBytes.getLength(), sal_False );
        
        sal_uInt16 nMethods = (sal_uInt16)aReader.getMethodCount();
        sal_uInt16 nFields  = (sal_uInt16)aReader.getFieldCount();
        
        vector< AttributeInit > * pAttributes = new vector< AttributeInit >( nFields );
        vector< MethodInit > * pMethods		  = new vector< MethodInit >( nMethods );
        
        OUString aInterfaceName( getName() );
        
        sal_Int32 nBaseOffset = BaseOffset(this).get();
        
        // all methods
        while (nMethods--)
        {
            OUString aMemberName( aReader.getMethodName( nMethods ) );
            OUStringBuffer aTypeName( aInterfaceName );
            aTypeName.appendAscii( RTL_CONSTASCII_STRINGPARAM("::") );
            aTypeName.append( aMemberName );
            
            RTMethodMode eMode = aReader.getMethodMode( nMethods );
            
            MethodInit & rInit = pMethods->operator[]( nMethods );
            
            rInit.aTypeName	   = aTypeName.makeStringAndClear();
            rInit.aMemberName  = aMemberName;
            rInit.aReturnTypeName = aReader.getMethodReturnType( nMethods ).replace( '/', '.' );
            rInit.nMethodIndex = nMethods;
            rInit.bOneWay	   = (eMode == RT_MODE_ONEWAY || eMode == RT_MODE_ONEWAY_CONST);
        }
        
        // all fields
        while (nFields--)
        {
            OUString aMemberName( aReader.getFieldName( nFields ) );
            OUString aMemberType( aReader.getFieldType( nFields ).replace( '/', '.' ) );
            OUStringBuffer aTypeName( aInterfaceName );
            aTypeName.appendAscii( RTL_CONSTASCII_STRINGPARAM("::") );
            aTypeName.append( aMemberName );
            
            AttributeInit & rInit = pAttributes->operator[]( nFields );
            
            rInit.aTypeName		  = aTypeName.makeStringAndClear();
            rInit.aMemberName	  = aMemberName;
            rInit.aMemberTypeName = aMemberType;
            rInit.bReadOnly		  = (aReader.getFieldAccess( nFields ) == RT_ACCESS_READONLY);
        }
        
        ClearableMutexGuard aGuard( getMutex() );
        if (_pMethods)
        {
            aGuard.clear();
            delete pAttributes;
            delete pMethods;
        }
        else
        {
            _nBaseOffset = nBaseOffset;
            _pAttributes = pAttributes;
            _pMethods	 = pMethods;
        }
    }
    
    // collect members
    sal_Int32 nAttributes = _pAttributes->size();
    sal_Int32 nMethods	  = _pMethods->size();
    
    Sequence< Reference< XInterfaceMemberTypeDescription > > aMembers( nAttributes + nMethods );
    Reference< XInterfaceMemberTypeDescription > * pMembers = aMembers.getArray();
    
    while (nMethods--)
    {
        MethodInit const & rInit = _pMethods->operator[]( nMethods );
        pMembers[nAttributes+nMethods] = new InterfaceMethodImpl(
            _xTDMgr, rInit.aTypeName, rInit.aMemberName,
            rInit.aReturnTypeName, _aBytes, rInit.nMethodIndex,
            rInit.bOneWay, _nBaseOffset+nAttributes+nMethods );
    }
    while (nAttributes--)
    {
        AttributeInit const & rInit = _pAttributes->operator[]( nAttributes );
        pMembers[nAttributes] = new InterfaceAttributeImpl(
            _xTDMgr, rInit.aTypeName, rInit.aMemberName, rInit.aMemberTypeName,
            rInit.bReadOnly, _nBaseOffset+nAttributes );
    }
    
    return aMembers;
}
//__________________________________________________________________________________________________
Sequence< Reference< XInterfaceTypeDescription2 > > InterfaceTypeDescriptionImpl::getBaseTypes()
    throw(::com::sun::star::uno::RuntimeException)
{
    if (_xBaseTDs.getLength() == 0 && _aBaseTypes.getLength() != 0)
    {
        try
        {
            Sequence< Reference< XInterfaceTypeDescription2 > > xBaseTDs(
                _aBaseTypes.getLength());
            bool bSuccess = true;
            for (sal_Int32 i = 0; i < _aBaseTypes.getLength(); ++i)
            {
                if (!(_xTDMgr->getByHierarchicalName( _aBaseTypes[i] )
                      >>= xBaseTDs[i]))
                {
                    bSuccess = false;
                    break;
                }
            }
            if (bSuccess)
            {
                MutexGuard aGuard( getMutex() );
                if (_xBaseTDs.getLength() == 0)
                {
                    _xBaseTDs = xBaseTDs;
                }
                return _xBaseTDs;
            }
        }
        catch (NoSuchElementException &)
        {
        }
        // never try again, if no base tds were found
        _aBaseTypes.realloc(0);
    }
    return _xBaseTDs;
}

}


