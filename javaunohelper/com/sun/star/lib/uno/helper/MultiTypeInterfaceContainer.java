/*************************************************************************
 *
 *  $RCSfile: MultiTypeInterfaceContainer.java,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: jl $ $Date: 2002-04-11 13:39:51 $
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

package com.sun.star.lib.uno.helper;
import com.sun.star.uno.Type;
import com.sun.star.lang.EventObject;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.Iterator;

public class MultiTypeInterfaceContainer
{
    
    private Map map= new HashMap();
    
    /** Creates a new instance of MultiTypeInterfaceContainer */
    public MultiTypeInterfaceContainer()
    {
    }
    
    /** only returns types which have at least one value in InterfaceContainer
     *  return value can contain an element null, if someone called
     *  addInterface (null, interf)
     */
    synchronized public Type[] getContainedTypes()
    {
        int size;
        Type[] retVal= null;
        
        if ( (size=map.size()) > 0)
        {
            Type [] arTypes= new Type[size];
            Iterator it= map.keySet().iterator();
            
            int countTypes= 0;
            while (it.hasNext())
            {
                Object key= it.next();
                InterfaceContainer cont= (InterfaceContainer) map.get(key);
                if (cont != null && cont.size() > 0)
                {
                    if (key == null)
                        arTypes[countTypes++]= new Type();
                    else if (key instanceof Type)
                        arTypes[countTypes++]= (Type) key;
                    else if (key instanceof Class)
                        arTypes[countTypes++]= new Type((Class) key);
                    else
                        arTypes[countTypes++]= new Type(key.getClass());
                }
            }
            
            if (countTypes != size)
            {
                retVal= new Type[countTypes];
                System.arraycopy(arTypes, 0, retVal, 0, countTypes);
            }
            else
                retVal= arTypes;
        }
        if (retVal == null)
            retVal= new Type[0];
        return retVal;
    }
    
    /** param key can be null */
    synchronized public InterfaceContainer getContainer(Type key)
    {
        InterfaceContainer retVal= null;
        Iterator it= map.keySet().iterator();
        while (it.hasNext())
        {
            Object obj= it.next();
            if (obj == null && key == null)
            {
                retVal= (InterfaceContainer) map.get(null);
                break;
            }
            else if( obj != null && obj.equals(key))
            {
                retVal= (InterfaceContainer) map.get(obj);
                break;
            }
        }
        return retVal;
    }
    
    
    synchronized public int addInterface(Object ckey, Object iface)
    {
        //If the key is a Type then it does not matter if the objects are different
        // if they represent the same type. This is because Types overrides hashCode and 
        // equals. For example:
        // Type a= new Type(XInterface.class);
        // Type b= new Type(XInterface.class);
        // Allthough a != b , the map interprets both as being the same.
        InterfaceContainer cont= (InterfaceContainer) map.get(ckey);
        if (cont != null)
        {
            cont.add(iface);
        }
        else
        {
            cont= new InterfaceContainer();
            cont.add(iface);
            map.put(ckey, cont);
        }
        return cont.size();
    }
  
    
    synchronized public int removeInterface(Type key, Object iface)
    {
        int retVal= 0;
        InterfaceContainer cont= (InterfaceContainer) map.get(key);
        if (cont != null)
        {
            cont.remove(iface);
            retVal= cont.size();
        }
        return retVal;
    }
    
    public void disposeAndClear(EventObject evt)
    {
        Iterator it= null;
        synchronized(this)
        {
            it= map.values().iterator();
        }
        while (it.hasNext() )
            ((InterfaceContainer) it.next()).disposeAndClear(evt);
    }
    
    synchronized public void clear()
    {
        Iterator it= map.values().iterator();
        while (it.hasNext())
            ((InterfaceContainer) it.next()).clear();
    }
}
