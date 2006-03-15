/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: codemaker.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: vg $ $Date: 2006-03-15 09:09:10 $
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

#ifndef INCLUDED_CODEMAKER_CODEMAKER_HXX
#define INCLUDED_CODEMAKER_CODEMAKER_HXX

#ifndef _SAL_CONFIG_H_
#include "sal/config.h"
#endif
#ifndef INCLUDED_CODEMAKER_UNOTYPES_HXX
#include "codemaker/unotype.hxx"
#endif
#ifndef INCLUDED_registry_types_h
#include "registry/types.h"
#endif
#ifndef _SAL_TYPES_H_
#include "sal/types.h"
#endif

#include <vector>

namespace rtl {
    class OString;
    class OUString;
}
class TypeManager;

namespace codemaker { 

rtl::OString convertString(rtl::OUString const & string);

codemaker::UnoType::Sort decomposeAndResolve(
    TypeManager const & manager, rtl::OString const & type,
    bool resolveTypedefs, bool allowVoid, bool allowExtraEntities,
    RTTypeClass * typeClass, rtl::OString * name, sal_Int32 * rank,
    std::vector< rtl::OString > * arguments);

}

#endif // INCLUDED_CODEMAKER_CODEMAKER_HXX
