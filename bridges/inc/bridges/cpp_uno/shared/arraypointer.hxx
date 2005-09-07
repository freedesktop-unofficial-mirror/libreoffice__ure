/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: arraypointer.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-07 22:08:53 $
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

#ifndef INCLUDED_BRIDGES_CPP_UNO_SHARED_ARRAYPOINTER_HXX
#define INCLUDED_BRIDGES_CPP_UNO_SHARED_ARRAYPOINTER_HXX

#ifndef _SAL_CONFIG_H_
#include "sal/config.h"
#endif

namespace bridges { namespace cpp_uno { namespace shared {

/**
 * A simple smart pointer that holds an array until it is being released.
 */
template< typename T > class ArrayPointer {
public:
    ArrayPointer(T * p): p_(p) {}

    ~ArrayPointer() { delete[] p_; }

    T * release() { T * t = p_; p_ = 0; return t; }

private:
    ArrayPointer(ArrayPointer &); // not defined
    void operator =(ArrayPointer &); // not defined

    T * p_;
};

} } }

#endif
