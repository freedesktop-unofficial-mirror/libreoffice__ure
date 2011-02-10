/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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
#include "precompiled_stoc.hxx"
#include <cppuhelper/queryinterface.hxx>
#include <uno/any2.h>

#include "base.hxx"

namespace stoc_corefl
{

#ifdef TEST_LIST_CLASSES
ClassNameList g_aClassNames;
#endif

//--------------------------------------------------------------------------------------------------
Mutex & getMutexAccess()
{
    static Mutex * s_pMutex = 0;
    if (! s_pMutex)
    {
        MutexGuard aGuard( Mutex::getGlobalMutex() );
        if (! s_pMutex)
        {
            static Mutex s_aMutex;
            s_pMutex = &s_aMutex;
        }
    }
    return *s_pMutex;
}

//__________________________________________________________________________________________________
IdlClassImpl::IdlClassImpl( IdlReflectionServiceImpl * pReflection,
                            const OUString & rName, typelib_TypeClass eTypeClass,
                            typelib_TypeDescription * pTypeDescr )
    : _pReflection( pReflection )
    , _aName( rName )
    , _eTypeClass( (TypeClass)eTypeClass )
    , _pTypeDescr( pTypeDescr )
{
    if (_pReflection)
        _pReflection->acquire();
    if (_pTypeDescr)
    {
        typelib_typedescription_acquire( _pTypeDescr );
        if (! _pTypeDescr->bComplete)
            typelib_typedescription_complete( &_pTypeDescr );
    }
    
#ifdef TEST_LIST_CLASSES
    ClassNameList::const_iterator iFind( find( g_aClassNames.begin(), g_aClassNames.end(), _aName ) );
    OSL_ENSURE( iFind == g_aClassNames.end(), "### idl class already exists!" );
    g_aClassNames.push_front( _aName );
#endif
}
//__________________________________________________________________________________________________
IdlClassImpl::~IdlClassImpl()
{
    if (_pTypeDescr)
        typelib_typedescription_release( _pTypeDescr );
    if (_pReflection)
        _pReflection->release();
    
#ifdef TEST_LIST_CLASSES
    ClassNameList::iterator iFind( find( g_aClassNames.begin(), g_aClassNames.end(), _aName ) );
    OSL_ENSURE( iFind != g_aClassNames.end(), "### idl class does not exist!" );
    g_aClassNames.erase( iFind );
#endif
}

// XIdlClassImpl default implementation
//__________________________________________________________________________________________________
TypeClass IdlClassImpl::getTypeClass()
    throw(::com::sun::star::uno::RuntimeException)
{
    return _eTypeClass;
}
//__________________________________________________________________________________________________
OUString IdlClassImpl::getName()
    throw(::com::sun::star::uno::RuntimeException)
{
    return _aName;
}
//__________________________________________________________________________________________________
sal_Bool IdlClassImpl::equals( const Reference< XIdlClass >& xType )
    throw(::com::sun::star::uno::RuntimeException)
{
    return (xType.is() &&
            (xType->getTypeClass() == _eTypeClass) && (xType->getName() == _aName));
}

static sal_Bool s_aAssignableFromTab[11][11] =
{
                         /* from CH,BO,BY,SH,US,LO,UL,HY,UH,FL,DO */
/* TypeClass_CHAR */			{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/* TypeClass_BOOLEAN */			{ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/* TypeClass_BYTE */			{ 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
/* TypeClass_SHORT */			{ 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0 },
/* TypeClass_UNSIGNED_SHORT */	{ 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0 },
/* TypeClass_LONG */			{ 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
/* TypeClass_UNSIGNED_LONG */	{ 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
/* TypeClass_HYPER */			{ 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
/* TypeClass_UNSIGNED_HYPER */	{ 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
/* TypeClass_FLOAT */			{ 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
/* TypeClass_DOUBLE */			{ 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};
//__________________________________________________________________________________________________
sal_Bool IdlClassImpl::isAssignableFrom( const Reference< XIdlClass > & xType )
    throw(::com::sun::star::uno::RuntimeException)
{
    TypeClass eAssign = getTypeClass();
    if (equals( xType ) || eAssign == TypeClass_ANY) // default shot
    {
        return sal_True;
    }
    else
    {
        TypeClass eFrom	  = xType->getTypeClass();
        if (eAssign > TypeClass_VOID && eAssign < TypeClass_STRING &&
            eFrom > TypeClass_VOID && eFrom < TypeClass_STRING)
        {
            return s_aAssignableFromTab[eAssign-1][eFrom-1];
        }
    }
    return sal_False;
}
//__________________________________________________________________________________________________
void IdlClassImpl::createObject( Any & rObj )
    throw(::com::sun::star::uno::RuntimeException)
{
    rObj.clear();
    uno_any_destruct( &rObj, reinterpret_cast< uno_ReleaseFunc >(cpp_release) );
    uno_any_construct( &rObj, 0, getTypeDescr(), 0 );
}

// what TODO ????
//__________________________________________________________________________________________________
Sequence< Reference< XIdlClass > > IdlClassImpl::getClasses()
    throw(::com::sun::star::uno::RuntimeException)
{
    OSL_ENSURE( sal_False, "### unexpected use!" );
    return Sequence< Reference< XIdlClass > >();
}
//__________________________________________________________________________________________________
Reference< XIdlClass > IdlClassImpl::getClass( const OUString & )
    throw(::com::sun::star::uno::RuntimeException)
{
    OSL_ENSURE( sal_False, "### unexpected use!" );
    return Reference< XIdlClass >();
}
//__________________________________________________________________________________________________
Sequence< Reference< XIdlClass > > IdlClassImpl::getInterfaces()
    throw(::com::sun::star::uno::RuntimeException)
{
//  	OSL_ENSURE( sal_False, "### unexpected use!" );
    return Sequence< Reference< XIdlClass > >();
}

// structs, interfaces
//__________________________________________________________________________________________________
Sequence< Reference< XIdlClass > > IdlClassImpl::getSuperclasses() throw(::com::sun::star::uno::RuntimeException)
{
    return Sequence< Reference< XIdlClass > >();
}
// structs
//__________________________________________________________________________________________________
Reference< XIdlField > IdlClassImpl::getField( const OUString & )
    throw(::com::sun::star::uno::RuntimeException)
{
    return Reference< XIdlField >();
}
//__________________________________________________________________________________________________
Sequence< Reference< XIdlField > > IdlClassImpl::getFields()
    throw(::com::sun::star::uno::RuntimeException)
{
    return Sequence< Reference< XIdlField > >();
}
// interfaces
//__________________________________________________________________________________________________
Uik IdlClassImpl::getUik()
    throw(::com::sun::star::uno::RuntimeException)
{
    return Uik();
}
//__________________________________________________________________________________________________
Reference< XIdlMethod > IdlClassImpl::getMethod( const OUString & )
    throw(::com::sun::star::uno::RuntimeException)
{
    return Reference< XIdlMethod >();
}
//__________________________________________________________________________________________________
Sequence< Reference< XIdlMethod > > IdlClassImpl::getMethods()
    throw(::com::sun::star::uno::RuntimeException)
{
    return Sequence< Reference< XIdlMethod > >();
}
// array
//__________________________________________________________________________________________________
Reference< XIdlClass > IdlClassImpl::getComponentType()
    throw(::com::sun::star::uno::RuntimeException)
{
    return Reference< XIdlClass >();
}
//__________________________________________________________________________________________________
Reference< XIdlArray > IdlClassImpl::getArray()
    throw(::com::sun::star::uno::RuntimeException)
{
    return Reference< XIdlArray >();
}


//##################################################################################################
//##################################################################################################
//##################################################################################################


//__________________________________________________________________________________________________
IdlMemberImpl::IdlMemberImpl( IdlReflectionServiceImpl * pReflection, const OUString & rName,
                              typelib_TypeDescription * pTypeDescr,
                              typelib_TypeDescription * pDeclTypeDescr )
    : _pReflection( pReflection )
    , _aName( rName )
    , _pTypeDescr( pTypeDescr )
    , _pDeclTypeDescr( pDeclTypeDescr )
{
    _pReflection->acquire();
    typelib_typedescription_acquire( _pTypeDescr );
    if (! _pTypeDescr->bComplete)
        typelib_typedescription_complete( &_pTypeDescr );
    typelib_typedescription_acquire( _pDeclTypeDescr );
    if (! _pDeclTypeDescr->bComplete)
        typelib_typedescription_complete( &_pDeclTypeDescr );
}
//__________________________________________________________________________________________________
IdlMemberImpl::~IdlMemberImpl()
{
    typelib_typedescription_release( _pDeclTypeDescr );
    typelib_typedescription_release( _pTypeDescr );
    _pReflection->release();
}

// XIdlMember
//__________________________________________________________________________________________________
Reference< XIdlClass > IdlMemberImpl::getDeclaringClass()
    throw(::com::sun::star::uno::RuntimeException)
{
    if (! _xDeclClass.is())
    {
        Reference< XIdlClass > xDeclClass( getReflection()->forType( getDeclTypeDescr() ) );
        MutexGuard aGuard( getMutexAccess() );
        if (! _xDeclClass.is())
            _xDeclClass = xDeclClass;
    }
    return _xDeclClass;
}
//__________________________________________________________________________________________________
OUString IdlMemberImpl::getName()
    throw(::com::sun::star::uno::RuntimeException)
{
    return _aName;
}

}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
