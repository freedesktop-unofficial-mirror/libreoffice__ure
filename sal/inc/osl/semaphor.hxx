/*************************************************************************
 *
 *  $RCSfile: semaphor.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: obr $ $Date: 2001-11-12 15:51:50 $
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

#ifndef _OSL_SEMAPHORE_HXX_
#define _OSL_SEMAPHORE_HXX_

#ifdef __cplusplus

#include <osl/semaphor.h>


namespace osl
{     

    class Semaphore {
        oslSemaphore semaphore;

    public:

        /** Creates a semaphore.
            @param InitialCount denotes the starting value the semaphore. If you set it to
            zero, the first acquire() blocks. Otherwise InitialCount acquire()s  are
            immedeatly  successfull.
            @seealso ::osl_createSemaphore()
        */
        
        Semaphore(sal_uInt32 initialCount)
        {
            semaphore = osl_createSemaphore(initialCount);
        }

        /** Release the OS-structures and free semaphore data-structure
            @seealso ::osl_destroySemaphore()
        */
        ~Semaphore() 
        {
            osl_destroySemaphore(semaphore);
        }

        /** Decreases the count of the semaphore object. 
        
            It will block if it tries to decrease below zero.
            @return sal_False if the system-call failed
            @seealso ::osl_acquireSemaphore()
        */
        sal_Bool acquire()
        {
            return osl_acquireSemaphore(semaphore);
        }

        /** Tries to decreases the count of the semaphore.
        
            @return sal_False if it would decrease the count below zero
            (acquire() would block). If it could successfully 
            decrease the count, it will return sal_True.
            @seealso ::osl_tryToAcquireSemaphore()
        */
        sal_Bool tryToAcquire()
        {
            return osl_tryToAcquireSemaphore(semaphore);
        }

        /** Increases the count.
            @return sal_False if the system-call failed.
            @seealso ::osl_releaseSemaphore()
        */
        sal_Bool release()
        {
            return osl_releaseSemaphore(semaphore);
        }
    };
}

#endif  /* __cplusplus */
#endif  /* _OSL_SEMAPHORE_HXX_  */
