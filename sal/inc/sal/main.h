/*************************************************************************
 *
 *  $RCSfile: main.h,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hro $ $Date: 2001-02-22 15:53:46 $
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

#ifndef _SAL_MAIN_H_
#define _SAL_MAIN_H_

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Prototype of sal main entry */

int SAL_CALL sal_main();


/* Definition macros for CRT entries */

#ifdef SAL_W32

/* Sorry but this is neccessary cause HINSTANCE is a typedef that differs (C++ causes an error) */

#if 0

#ifndef _WINDOWS_
#include <windows.h>
#endif

#else /* Simulated what windows.h does */

#ifndef WINAPI
#	define WINAPI	__stdcall
#endif

#if !defined(DECLARE_HANDLE)
#	ifdef STRICT
#		define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
#	else
#		define DECLARE_HANDLE(name) typedef HANDLE name
#	endif
DECLARE_HANDLE(HINSTANCE);
#endif

#endif


#define SAL_DEFINE_CRT_ENTRY() \
int __cdecl main() \
{ \
    return sal_main(); \
} \
int WINAPI WinMain( HINSTANCE _hinst, HINSTANCE _dummy, char* _cmdline, int _nshow ) \
{ \
    return sal_main(); \
}


#else	/* ! SAL_W32 */

#define SAL_DEFINE_CRT_ENTRY() \
int main() \
{ \
    return sal_main(); \
} \

#endif

/* Implementation macro */

#define SAL_IMPLEMENT_MAIN() \
    SAL_DEFINE_CRT_ENTRY() \
    int SAL_CALL sal_main()


/*
    "How to use" Example:


    #include <sal/main.h>


    SAL_IMPLEMENT_MAIN()
    {
        DoSomething();

        return 0;
    }

*/


#ifdef __cplusplus
}	/* extern "C" */
#endif

#endif	/* _SAL_MAIN_H_ */

