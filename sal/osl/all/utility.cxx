/*************************************************************************
 *
 *  $RCSfile: utility.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: mfe $ $Date: 2001-02-27 15:50:41 $
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


#include "rtl/ustring.hxx"
#include "osl/time.h"
#include <stdio.h>


/*
 *   mfe : maybe it would be wishful to include initialization
 *         of the global timer in dllmain or _init directly.
 *         But noneoftheless this (should) work too.
 */
namespace osl
{

class OGlobalTimer
{
    
public:
    
    OGlobalTimer() {
        getTime();
    }
    
    sal_uInt32 getTime()
    {
        return osl_getGlobalTimer();
    }

    
};

static OGlobalTimer aGlobalTimer;

}


extern "C" 
{
void debug_ustring(rtl_uString* ustr)
{
    sal_Char* psz=0;
    rtl_String* str=0;

    if ( ustr != 0 )
    {
        rtl_uString2String( &str,
                            rtl_uString_getStr(ustr),
                            rtl_uString_getLength(ustr),
                            RTL_TEXTENCODING_UTF8,
                            OUSTRING_TO_OSTRING_CVTFLAGS );

        psz = rtl_string_getStr(str);
    }

    fprintf(stderr,"'%s'\n",psz);

    if ( str != 0 )
    {
        rtl_string_release(str);
    }

    return;
}

}

void debug_oustring(rtl::OUString& ustr)
{

    debug_ustring(ustr.pData);

    return;
}
