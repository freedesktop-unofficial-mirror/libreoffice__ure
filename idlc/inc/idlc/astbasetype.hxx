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
#ifndef _IDLC_ASTBASETYPE_HXX_
#define _IDLC_ASTBASETYPE_HXX_

#include <idlc/asttype.hxx>
#include <idlc/astexpression.hxx>


class AstBaseType : public AstType
{
public:
    AstBaseType(const ExprType type, const ::rtl::OString& name, AstScope* pScope)
        : AstType(NT_predefined, name, pScope)
        , m_exprType(type)
        {}

    virtual ~AstBaseType() {}

    virtual bool isUnsigned() const {
        switch (m_exprType) {
        case ET_ushort:
        case ET_ulong:
        case ET_uhyper:
            return true;
        default:
            return false;
        }
    }

    ExprType getExprType() const
        { return m_exprType; }
private:
    const ExprType m_exprType;
};

#endif // _IDLC_ASTBASETYPE_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
