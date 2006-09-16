/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: starter.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-16 16:04:25 $
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
#include "precompiled_bridges.hxx"

#include <stdio.h>
#include "share.h"


using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

static void some_more(  t_throws_exc p )
{
    (*p)();
}

extern "C" void SAL_CALL start( t_throws_exc p )
{
    try
    {
        some_more( p );
    }
    catch (lang::IllegalArgumentException & exc)
    {
        OString msg( OUStringToOString( exc.Message, RTL_TEXTENCODING_ASCII_US ) );
        printf( "starter.cxx: caught IllegalArgumentException: %s\n", msg.getStr() );
    }
    catch (Exception & exc)
    {
        OString msg( OUStringToOString( exc.Message, RTL_TEXTENCODING_ASCII_US ) );
        printf( "starter.cxx: caught some UNO exc: %s\n", msg.getStr() );
    }
    catch (...)
    {
        printf( "starter.cxx: caught something\n" );
    }
}
