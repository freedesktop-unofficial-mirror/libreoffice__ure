/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: counter.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: ihi $ $Date: 2006-08-24 10:42:45 $
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
#ifndef _BRIDGES_REMOTE_COUNTER_H_
#define _BRIDGES_REMOTE_COUNTER_H_
#include <stdio.h>
#if OSL_DEBUG_LEVEL > 1
struct MyCounter
{
    MyCounter( sal_Char const *pName ) :
        m_nCounter( 0 ),
        m_pName ( pName )
        {
        }
    ~MyCounter()
        {
            if( m_nCounter ) {
                printf(
                    "%s : %ld left\n", m_pName,
                    sal::static_int_cast< long >(m_nCounter) );
            }
        }
    void acquire()
        { m_nCounter ++; }
    void release()
        { m_nCounter --; }


    sal_Int32 m_nCounter;
    sal_Char const *m_pName;
};
#endif

#endif
