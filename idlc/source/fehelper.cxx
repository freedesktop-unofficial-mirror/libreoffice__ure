/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: fehelper.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 08:14:06 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_idlc.hxx"
#ifndef _IDLC_FEHELPER_HXX_
#include <idlc/fehelper.hxx>
#endif
#ifndef _IDLC_ERRORHANDLER_HXX_
#include <idlc/errorhandler.hxx>
#endif
#ifndef _IDLC_ASTARRAY_HXX_
#include <idlc/astarray.hxx>
#endif
#include "idlc/idlc.hxx"

using namespace ::rtl;

FeDeclarator::FeDeclarator(const OString& name, DeclaratorType declType, AstDeclaration* pComplPart)
    : m_pComplexPart(pComplPart)
    , m_name(name)
    , m_declType(declType)
{
}	

FeDeclarator::~FeDeclarator()
{
}	

sal_Bool FeDeclarator::checkType(AstDeclaration const * type)
{
    OString tmp(m_name);
    sal_Int32 count = m_name.lastIndexOf( ':' );
    if( count != -1 )
        tmp = m_name.copy( count+1 );

    if (tmp == type->getLocalName())
        return sal_False;
    else
        return sal_True;
}	

AstType const * FeDeclarator::compose(AstDeclaration const * pDecl)
{
    AstArray*	pArray;
    AstType*	pType;

    if ( pDecl == 0 )
    {
        return NULL;
    }
    if ( !pDecl->isType() ) 
    {
        idlc()->error()->noTypeError(pDecl);
        return NULL;
    }
    pType = (AstType*)pDecl;
    if (m_declType == FD_simple || m_pComplexPart == NULL) 
        return pType;

    if (m_pComplexPart->getNodeType() == NT_array) 
    {
        pArray = (AstArray*)m_pComplexPart;
        pArray->setType(pType);

        // insert array type in global scope
        AstScope* pScope = idlc()->scopes()->bottom();
        if ( pScope )
        {
            AstDeclaration* pDecl2 = pScope->addDeclaration(pArray);
            if ( (AstDeclaration*)pArray != pDecl2 )
            {
                delete m_pComplexPart;
                m_pComplexPart = pDecl2;	
            }
        }
        return pArray;
    }

    return NULL; // return through this statement should not happen
}	

FeInheritanceHeader::FeInheritanceHeader(
    NodeType nodeType, ::rtl::OString* pName, ::rtl::OString* pInherits,
    std::vector< rtl::OString > * typeParameters)
    : m_nodeType(nodeType)
    , m_pName(pName)
    , m_pInherits(NULL)
{
    if (typeParameters != 0) {
        m_typeParameters = *typeParameters;
    }
    initializeInherits(pInherits);
}

void FeInheritanceHeader::initializeInherits(::rtl::OString* pInherits)
{
    if ( pInherits )
    {
        AstScope* pScope = idlc()->scopes()->topNonNull();
        AstDeclaration* pDecl = pScope->lookupByName(*pInherits);
        if ( pDecl )
        {
            AstDeclaration const * resolved = resolveTypedefs(pDecl);
            if ( resolved->getNodeType() == getNodeType()
                 && (resolved->getNodeType() != NT_interface
                     || static_cast< AstInterface const * >(
                         resolved)->isDefined()) )
            {
                if ( idlc()->error()->checkPublished( pDecl ) )
                {
                    m_pInherits = pDecl;
                }
            }
            else
            {
                idlc()->error()->inheritanceError(
                    getNodeType(), getName(), pDecl);
            }
        }
        else
        {
            idlc()->error()->lookupError(*pInherits);
        }
    }
}
