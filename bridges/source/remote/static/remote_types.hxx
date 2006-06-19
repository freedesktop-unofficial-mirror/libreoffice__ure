/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: remote_types.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 23:51:37 $
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
#ifndef _BRIDGES_REMOTE_TYPES_HXX_
#define _BRIDGES_REMOTE_TYPES_HXX_

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif
#ifndef _TYPELIB_TYPEDESCRIPTION_H_
#include <typelib/typedescription.h>
#endif

namespace bridges_remote
{
    
inline sal_Bool SAL_CALL remote_relatesToInterface( typelib_TypeDescription *pTypeDescr );
sal_Bool SAL_CALL remote_relatesToInterface2( typelib_TypeDescription * pTypeDescr );


/** Determines whether given type might relate or relates to an interface,
    i.e. values of this type are interface or may contain interface(s).<br>
    @param pTypeDescr type description of type
    @return true if type might relate to an interface, false otherwise
*/
inline sal_Bool SAL_CALL remote_relatesToInterface( typelib_TypeDescription * pTypeDescr )
{
    switch (pTypeDescr->eTypeClass)
    {
    case typelib_TypeClass_SEQUENCE:
        switch (((typelib_IndirectTypeDescription *)pTypeDescr)->pType->eTypeClass)
        {
        case typelib_TypeClass_INTERFACE:
        case typelib_TypeClass_UNION: // might relate to interface
        case typelib_TypeClass_ANY: // might relate to interface
            return sal_True;
        case typelib_TypeClass_SEQUENCE:
        case typelib_TypeClass_STRUCT:
        case typelib_TypeClass_EXCEPTION:
            return remote_relatesToInterface2( pTypeDescr );
        default:
            return sal_False;
        }
    case typelib_TypeClass_STRUCT:
    case typelib_TypeClass_EXCEPTION:
        return remote_relatesToInterface2( pTypeDescr );
    case typelib_TypeClass_UNION: // might relate to interface
    case typelib_TypeClass_ANY: // might relate to interface
    case typelib_TypeClass_INTERFACE:
        return sal_True;
    default:
        return sal_False;
    }
}

/** Determines whether given type is a cpp simple type, e.g. int, enum.<br>
    @param pTypeDescr type description of type
    @return true if type is a cpp simple type, false otherwise
*/
inline sal_Bool SAL_CALL remote_isSimpleType( typelib_TypeDescription * pTypeDescr )
{
    return (pTypeDescr->eTypeClass <= typelib_TypeClass_ENUM &&
            pTypeDescr->eTypeClass != typelib_TypeClass_STRING &&
            pTypeDescr->eTypeClass != typelib_TypeClass_ANY &&
            pTypeDescr->eTypeClass != typelib_TypeClass_TYPE);
}

}
#endif
