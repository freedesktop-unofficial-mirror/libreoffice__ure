/*************************************************************************
 *
 *  $RCSfile: aststruct.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2004-03-30 16:46:04 $
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
#ifndef _IDLC_ASTSTRUCT_HXX_
#include <idlc/aststruct.hxx>
#endif
#ifndef _IDLC_ASTMember_HXX_
#include <idlc/astmember.hxx>
#endif

#include "registry/version.h"
#include "registry/writer.hxx"

using namespace ::rtl;

AstStruct::AstStruct(const OString& name, AstStruct* pBaseType, AstScope* pScope)
    : AstType(NT_struct, name, pScope)
    , AstScope(NT_struct)
    , m_pBaseType(pBaseType)
{
}
    
AstStruct::AstStruct(const NodeType type, 
                        const OString& name, 
                       AstStruct* pBaseType,
                     AstScope* pScope)
    : AstType(type, name, pScope)
    , AstScope(type)
    , m_pBaseType(pBaseType)
{
}

AstStruct::~AstStruct()
{	
}	

sal_Bool AstStruct::dump(RegistryKey& rKey)
{
    RegistryKey localKey;
    if (rKey.createKey( OStringToOUString(getFullName(), RTL_TEXTENCODING_UTF8 ), localKey))
    {
        fprintf(stderr, "%s: warning, could	not create key '%s' in '%s'\n",
                idlc()->getOptions()->getProgramName().getStr(),
                getFullName().getStr(), OUStringToOString(rKey.getRegistryName(), RTL_TEXTENCODING_UTF8).getStr());
        return sal_False;
    }

    sal_uInt16 nMember = getNodeCount(NT_member);

    RTTypeClass typeClass = RT_TYPE_STRUCT;
    if ( getNodeType() == NT_exception )
        typeClass = RT_TYPE_EXCEPTION;

    typereg::Writer aBlob(
        TYPEREG_VERSION_0, getDocumentation(),
        OStringToOUString(getFileName(), RTL_TEXTENCODING_UTF8), typeClass,
        OStringToOUString(getRelativName(), RTL_TEXTENCODING_UTF8),
        m_pBaseType == 0 ? 0 : 1, nMember, 0, 0);
    if (m_pBaseType != 0) {
        aBlob.setSuperTypeName(
            0,
            OStringToOUString(
                m_pBaseType->getRelativName(), RTL_TEXTENCODING_UTF8));
    }

    if ( nMember > 0 )
    {
        DeclList::const_iterator iter = getIteratorBegin();
        DeclList::const_iterator end = getIteratorEnd();
        AstDeclaration* pDecl = NULL;
        AstMember* 	pMember = NULL;
        OUString 	docu;
        sal_uInt16 	index = 0;
        while ( iter != end )
        {
            pDecl = *iter;
            if ( pDecl->getNodeType() == NT_member )
            {
                pMember = (AstMember*)pDecl;
                aBlob.setFieldData(
                    index++, pMember->getDocumentation(), OUString(),
                    RT_ACCESS_READWRITE,
                    OStringToOUString(
                        pMember->getLocalName(), RTL_TEXTENCODING_UTF8),
                    OStringToOUString(
                        pMember->getType()->getRelativName(),
                        RTL_TEXTENCODING_UTF8),
                    RTConstValue());
            }
            ++iter;
        }
    }

    sal_uInt32 aBlobSize;
    void const * pBlob = aBlob.getBlob(&aBlobSize);
    
    if (localKey.setValue(OUString(), RG_VALUETYPE_BINARY, 
                            (RegValue)pBlob, aBlobSize))
    {
        fprintf(stderr, "%s: warning, could	not set value of key \"%s\" in %s\n",
                idlc()->getOptions()->getProgramName().getStr(),
                getFullName().getStr(), OUStringToOString(localKey.getRegistryName(), RTL_TEXTENCODING_UTF8).getStr());
        return sal_False;
    }				

    return sal_True;
}	

