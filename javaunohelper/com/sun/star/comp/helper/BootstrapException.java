/*************************************************************************
 *
 *  $RCSfile: BootstrapException.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2004-11-15 13:31:04 $
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

package com.sun.star.comp.helper;

/**
 * BootstrapException is a checked exception that wraps an exception
 * thrown by the original target.
 * 
 * @since UDK 3.1.0
 */
public class BootstrapException extends java.lang.Exception {
    
    /**
     * This field holds the target exception.
     */
    private Exception m_target = null;
    
    /**
     * Constructs a <code>BootstrapException</code> with <code>null</code> as
     * the target exception.
     */
    public BootstrapException() {
        super();
    }

    /**
     * Constructs a <code>BootstrapException</code> with the specified
     * detail message.
     *
     * @param  message   the detail message
     */
    public BootstrapException( String message ) {
        super( message );
    }    

    /**
     * Constructs a <code>BootstrapException</code> with the specified
     * detail message and a target exception.
     *
     * @param  message   the detail message
     * @param  target    the target exception
     */
    public BootstrapException( String message, Exception target ) {
        super( message );
        m_target = target;
    }
    
    /**
     * Constructs a <code>BootstrapException</code> with a target exception.
     *
     * @param  target    the target exception     
     */
    public BootstrapException( Exception target ) {
        super();
        m_target = target;
    }
    
    /**
     * Get the thrown target exception.
     *
     * @return the target exception
     */
    public Exception getTargetException() {
        return m_target;
    }    
}
