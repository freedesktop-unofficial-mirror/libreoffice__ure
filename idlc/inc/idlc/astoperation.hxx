/*************************************************************************
 *
 *  $RCSfile: astoperation.hxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: jsc $ $Date: 2001-03-15 12:23:01 $
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
#ifndef _IDLC_ASTOPERATION_HXX_
#define _IDLC_ASTOPERATION_HXX_

#ifndef _IDLC_ASTDECLARATION_HXX_
#include <idlc/astdeclaration.hxx>
#endif
#ifndef _IDLC_ASTSCOPE_HXX_
#include <idlc/astscope.hxx>
#endif

#define OP_NONE			0x0000
#define OP_ONEWAY		0x0001

class AstType;

class AstOperation : public AstDeclaration
                   , public AstScope
{
public:
    AstOperation(sal_uInt32 flags, AstType* pReturnType, const ::rtl::OString& name, AstScope* pScope)
        : AstDeclaration(NT_operation, name, pScope)
        , AstScope(NT_operation)
        , m_flags(flags)
        , m_pReturnType(pReturnType)
        {}
    virtual ~AstOperation() {}

    sal_Bool isOneway()
        { return ((m_flags & OP_ONEWAY) == OP_ONEWAY); }
    sal_Bool isVoid();
    AstType* getReturnType()
        { return m_pReturnType; }

    void addExceptions(StringList* pExceptions);
    const DeclList& getExceptions()
        { return m_exceptions; }
    sal_uInt16 nExceptions()
        { return m_exceptions.size(); }

    sal_Bool dumpBlob(RegistryTypeWriter& rBlob, sal_uInt16 index);

    // scope management
    virtual AstDeclaration* addDeclaration(AstDeclaration* pDecl);
private:
    sal_uInt32 	m_flags;
    AstType*	m_pReturnType;
    DeclList	m_exceptions;
};

#endif // _IDLC_ASTOPERATION_HXX_

