/*************************************************************************
 *
 *  $RCSfile: ThreadId.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2004-08-20 09:22:18 $
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

package com.sun.star.lib.uno.environments.remote;

import com.sun.star.uno.UnoRuntime;
import java.io.UnsupportedEncodingException;
import java.math.BigInteger;
import java.util.Arrays;

public final class ThreadId {
    static ThreadId createFresh() {
        BigInteger c;
        synchronized (PREFIX) {
            c = count;
            count = count.add(BigInteger.ONE);
        }
        try {
            return new ThreadId((PREFIX + c).getBytes("UTF-8"));
        } catch (UnsupportedEncodingException e) {
            throw new RuntimeException("this cannot happen: " + e);
        }
    }

    public ThreadId(byte[] id) {
        this.id = id;
    }

    public boolean equals(Object obj) {
        return obj instanceof ThreadId
            && Arrays.equals(id, ((ThreadId) obj).id);
    }

    public int hashCode() {
        int h = hash;
        if (h == 0) {
            // Same algorithm as java.util.List.hashCode (also see Java 1.5
            // java.util.Arrays.hashCode(byte[])):
            h = 1;
            for (int i = 0; i < id.length; ++i) {
                h = 31 * h + id[i];
            }
            hash = h;
        }
        return h;
    }

    public String toString() {
        StringBuffer b = new StringBuffer("[ThreadId:");
        for (int i = 0; i < id.length; ++i) {
            String n = Integer.toHexString(id[i] & 0xFF);
            if (n.length() == 1) {
                b.append('0');
            }
            b.append(n);
        }
        b.append(']');
        return b.toString();
    }

    public byte[] getBytes() {
        return id;
    }

    private static final String PREFIX
    = "java:" + UnoRuntime.getUniqueKey() + ":";
    private static BigInteger count = BigInteger.ZERO;

    private byte[] id;
    private int hash = 0;
}
