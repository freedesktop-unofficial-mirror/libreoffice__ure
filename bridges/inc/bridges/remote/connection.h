/*************************************************************************
 *
 *  $RCSfile: connection.h,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: jbu $ $Date: 2001-11-05 11:41:09 $
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
#ifndef _BRIDGES_REMOTE_CONNECTION_H_
#define _BRIDGES_REMOTE_CONNECTION_H_
#include <sal/types.h>

/** Specfies a C-interface for a bidirectional bytestream,
    which is used by a UNO remote environment.
 */
struct remote_Connection
{
    void ( SAL_CALL * acquire ) ( remote_Connection *);
    
    void ( SAL_CALL * release ) ( remote_Connection *);
    
    /**
      reads nSize bytes from the connection. This method blocks, until
      all bytes are available or an error occurs.
      @return Number of bytes read.
              If the return value is less than nSize, an unrecoverable
              i/o error has occured or the connection was closed.
                          
     */
    sal_Int32 (SAL_CALL * read)(remote_Connection *, sal_Int8 *pDest, sal_Int32 nSize );

    /**
      @return Number of bytes written.
              if the return value is less than nSize an unrecoverable
              i/o error has occured or the connection was closed.
     */
    sal_Int32 (SAL_CALL * write)(remote_Connection *, const sal_Int8 *pSource, sal_Int32 nSize );

    void ( SAL_CALL * flush ) ( remote_Connection * );

    /** closes the connection.
        Any read or write operation after this call shall not be served
        anymore. Any ongoing read or write operation must return immeadiatly after this call.
        The implementation should cope with multiple calls to this method.
     */
    void (SAL_CALL * close) ( remote_Connection * );
};

#endif
