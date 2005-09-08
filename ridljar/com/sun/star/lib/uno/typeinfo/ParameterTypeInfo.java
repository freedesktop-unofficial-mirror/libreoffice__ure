/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ParameterTypeInfo.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 13:19:29 $
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
package com.sun.star.lib.uno.typeinfo;

import com.sun.star.uno.Type;

public class ParameterTypeInfo extends TypeInfo
{
    protected int 		m_index;
    protected String 	m_methodName;
    private final Type m_unoType; // @since UDK 3.2

    /**
       Create a parameter type info with a UNO type that cannot unambiguously be
       represented as a Java&nbsp;1.2 type.

       @param name the name of this parameter; must not be <code>null</code>

       @param methodName the name of the method; must not be <code>null</code>

       @param index the index among the parameters

       @param flags any flags (<code>IN</code>, <code>OUT</code>,
       <code>UNSIGNED</code>, <code>ANY</code>, <code>INTERFACE</code>)

       @param unoType the exact UNO type; or <code>null</code> if the UNO type
       is already unambiguously represented by the Java&nbsp;1.2 type

       @since UDK 3.2
     */
    public ParameterTypeInfo(
        String name, String methodName, int index, int flags, Type unoType)
    {
        super(name, flags);
        m_index = index;
        m_methodName = methodName;
        m_unoType = unoType;
    }

    public ParameterTypeInfo(String name, String methodName, int index, int flags)
    {
        this(name, methodName, index, flags, null);
    }

    public String getMethodName()
    {
        return m_methodName;
    }

    public int getIndex()
    {
        return m_index;
    }

    public boolean isIN()
    {
        return ((m_flags & TypeInfo.IN) != 0 ||
                (m_flags & (TypeInfo.IN | TypeInfo.OUT)) == 0); // nothing set => IN
    }

    public boolean isOUT()
    {
        return (m_flags & TypeInfo.OUT) != 0;
    }

    public boolean isINOUT()
    {
        return (m_flags & (TypeInfo.IN | TypeInfo.OUT)) == (TypeInfo.IN | TypeInfo.OUT);
    }

    /**
       Get the exact UNO type of this parameter type info, in case it cannot
       unambiguously be represented as a Java&nbsp;1.2 type.

       <p>If this is an out or in&ndash;out parameter, the UNO type must be a
       sequence type, taking into account that such a parameter is represented
       in Java as a parameter of array type.</p>

       @return the exact UNO type of this parameter type info, or
       <code>null</code> if the UNO type is already unambiguously represented by
       the Java&nbsp;1.2 type

       @since UDK 3.2
     */
    public final Type getUnoType() {
        return m_unoType;
    }
}


