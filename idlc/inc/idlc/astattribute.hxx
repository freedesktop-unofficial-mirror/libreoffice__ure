/*************************************************************************
 *
 *  $RCSfile: astattribute.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2004-03-30 16:39:39 $
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
#ifndef _IDLC_ASTATTRIBUTE_HXX_
#define _IDLC_ASTATTRIBUTE_HXX_

#ifndef _IDLC_ASTDECLARATION_HXX_
#include <idlc/astdeclaration.hxx>
#endif
#include "idlc/astscope.hxx"

#include "registry/types.h"

namespace typereg { class Writer; }

class AstAttribute: public AstDeclaration, public AstScope {
public:
    AstAttribute(
        sal_uInt32 flags, AstType const * type, rtl::OString const & name,
        AstScope * scope):
        AstDeclaration(NT_attribute, name, scope), m_flags(flags),
        AstScope(NT_attribute), m_pType(type)
    {}

    AstAttribute(NodeType nodeType, sal_uInt32 flags, AstType const * pType, const ::rtl::OString& name, AstScope* pScope)
        : AstDeclaration(nodeType, name, pScope), AstScope(nodeType)
        , m_flags(flags)
        , m_pType(pType)
        {}
    virtual ~AstAttribute() {}

    void setExceptions(
        DeclList const * getter, DeclList const * setter)
    {
        if (getter != 0) {
            m_getExceptions = *getter;
        }
        if (setter != 0) {
            m_setExceptions = *setter;
        }
    }

    DeclList::size_type getGetExceptionCount() const
    { return m_getExceptions.size(); }

    DeclList::size_type getSetExceptionCount() const
    { return m_setExceptions.size(); }

    AstType const * getType() const
        { return m_pType; }
    const sal_Bool isReadonly()
        { return ((m_flags & AF_READONLY) == AF_READONLY); }
    const sal_Bool isOptional()
        { return ((m_flags & AF_OPTIONAL) == AF_OPTIONAL); }
    const sal_Bool isAttribute()
        { return ((m_flags & AF_ATTRIBUTE) == AF_ATTRIBUTE); }
    const sal_Bool isProperty()
        { return ((m_flags & AF_PROPERTY) == AF_PROPERTY); }
    const sal_Bool isBound()
        { return ((m_flags & AF_BOUND) == AF_BOUND); }
    const sal_Bool isMayBeVoid()
        { return ((m_flags & AF_MAYBEVOID) == AF_MAYBEVOID); }
    const sal_Bool isConstrained()
        { return ((m_flags & AF_CONSTRAINED) == AF_CONSTRAINED); }
    const sal_Bool isTransient()
        { return ((m_flags & AF_TRANSIENT) == AF_TRANSIENT); }
    const sal_Bool isMayBeAmbiguous()
        { return ((m_flags & AF_MAYBEAMBIGUOUS) == AF_MAYBEAMBIGUOUS); }
    const sal_Bool isMayBeDefault()
        { return ((m_flags & AF_MAYBEDEFAULT) == AF_MAYBEDEFAULT); }
    const sal_Bool isRemoveable()
        { return ((m_flags & AF_REMOVEABLE) == AF_REMOVEABLE); }

    sal_Bool dumpBlob(
        typereg::Writer & rBlob, sal_uInt16 index, sal_uInt16 * methodIndex);

private:
    void dumpExceptions(
        typereg::Writer & writer, DeclList const & exceptions,
        RTMethodMode flags, sal_uInt16  * methodIndex);

    const sal_uInt32 	m_flags;
    AstType const * m_pType;
    DeclList m_getExceptions;
    DeclList m_setExceptions;
};

#endif // _IDLC_ASTATTRIBUTE_HXX_

