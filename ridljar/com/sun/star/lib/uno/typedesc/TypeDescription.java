/*************************************************************************
 *
 *  $RCSfile: TypeDescription.java,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: hr $ $Date: 2003-08-07 14:37:35 $
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

package com.sun.star.lib.uno.typedesc;


import java.util.Enumeration;
import java.util.HashMap;
import java.util.Hashtable;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;


import com.sun.star.uno.Any;
import com.sun.star.uno.Enum;
import com.sun.star.uno.IFieldDescription;
import com.sun.star.uno.IMethodDescription;
import com.sun.star.uno.ITypeDescription;
import com.sun.star.uno.Type;
import com.sun.star.uno.TypeClass;
import com.sun.star.uno.Union;

import com.sun.star.lib.uno.typeinfo.AttributeTypeInfo;
import com.sun.star.lib.uno.typeinfo.MemberTypeInfo;
import com.sun.star.lib.uno.typeinfo.MethodTypeInfo;
import com.sun.star.lib.uno.typeinfo.ParameterTypeInfo;
import com.sun.star.lib.uno.typeinfo.TypeInfo;

/**
 * The Type class represents the IDL builtin type <code>type</code>.
 * <p>
 * The IDL type is not directly mapped to <code>java.lang.Class</code>, 
 * because it can be necessary to describe a type which is unknown 
 * to the java runtime system, e.g. for delaying the need of a class,
 * so that it is possible to generate it one the fly.
 * <p>
 * The current implementations holds various <code>static</code> helper
 * methods, which may be changed or moved in the furture, so please
 * do not use these methods.
 * <p>
 * @version 	$Revision: 1.15 $ $ $Date: 2003-08-07 14:37:35 $
 * @author 	    Kay Ramme
 * @since       UDK2.0
 */
public class TypeDescription implements ITypeDescription {
    /**
     * When set to true, enables various debugging output.
     */
    private static final boolean DEBUG = false;

    public static final TypeDescription __void_TypeDescription    = new TypeDescription(TypeClass.VOID,           "void", "[Ljava.lang.Void;", void.class);     // VOID
    public static final TypeDescription __char_TypeDescription    = new TypeDescription(TypeClass.CHAR,           "char", "[C", char.class);     // CHAR
    public static final TypeDescription __boolean_TypeDescription = new TypeDescription(TypeClass.BOOLEAN,        "boolean", "[Z", boolean.class);  // BOOLEAN
    public static final TypeDescription __byte_TypeDescription    = new TypeDescription(TypeClass.BYTE,           "byte", "[B", byte.class);     // BYTE
    public static final TypeDescription __short_TypeDescription   = new TypeDescription(TypeClass.SHORT,          "short", "[S", short.class);    // SHORT
    public static final TypeDescription __ushort_TypeDescription  = new TypeDescription(TypeClass.UNSIGNED_SHORT, "unsigned short", "[S", short.class);    // UNSIGNED SHORT
    public static final TypeDescription __long_TypeDescription    = new TypeDescription(TypeClass.LONG,           "long", "[I", int.class);     // LONG
    public static final TypeDescription __ulong_TypeDescription   = new TypeDescription(TypeClass.UNSIGNED_LONG,  "unsigned long", "[I", int.class);     // UNSIGNED_LONG
    public static final TypeDescription __hyper_TypeDescription   = new TypeDescription(TypeClass.HYPER,          "hyper", "[J", long.class);    // HYPER
    public static final TypeDescription __uhyper_TypeDescription  = new TypeDescription(TypeClass.UNSIGNED_HYPER, "unsigned hyper", "[J", long.class);    // UNSIGNED_HYPER
    public static final TypeDescription __float_TypeDescription   = new TypeDescription(TypeClass.FLOAT,          "float", "[F", float.class);    // FLOAT
    public static final TypeDescription __double_TypeDescription  = new TypeDescription(TypeClass.DOUBLE,         "double", "[D", double.class);   // DOUBLE
    public static final TypeDescription __string_TypeDescription  = new TypeDescription(TypeClass.STRING,         "string", "[Ljava.lang.String;", String.class);   // STRING
    public static final TypeDescription __type_TypeDescription    = new TypeDescription(TypeClass.TYPE,           "type", "[Lcom.sun.star.uno.Type;", Type.class);     // TYPE
    public static final TypeDescription __any_TypeDescription     = new TypeDescription(TypeClass.ANY,            "any", "[Ljava.lang.Object;", Object.class);       // ANY

    static private final HashMap __typeNameToTypeDescription = new HashMap();
    static {
        __typeNameToTypeDescription.put("void", __void_TypeDescription);
        __typeNameToTypeDescription.put("boolean", __boolean_TypeDescription);
        __typeNameToTypeDescription.put("char", __char_TypeDescription);
        __typeNameToTypeDescription.put("byte", __byte_TypeDescription);
        __typeNameToTypeDescription.put("short", __short_TypeDescription);
        __typeNameToTypeDescription.put("unsigned short",
                                        __ushort_TypeDescription);
        __typeNameToTypeDescription.put("long", __long_TypeDescription);
        __typeNameToTypeDescription.put("unsigned long",
                                        __ulong_TypeDescription);
        __typeNameToTypeDescription.put("hyper", __hyper_TypeDescription);
        __typeNameToTypeDescription.put("unsigned hyper",
                                        __uhyper_TypeDescription);
        __typeNameToTypeDescription.put("float", __float_TypeDescription);
        __typeNameToTypeDescription.put("double", __double_TypeDescription);
        __typeNameToTypeDescription.put("string", __string_TypeDescription);
        __typeNameToTypeDescription.put("type", __type_TypeDescription);
        __typeNameToTypeDescription.put("any", __any_TypeDescription);
    }

    static private final HashMap __classToTypeDescription = new HashMap();
    static {
        __classToTypeDescription.put(void.class, __void_TypeDescription);
        __classToTypeDescription.put(Void.class, __void_TypeDescription);
        __classToTypeDescription.put(boolean.class, __boolean_TypeDescription);
        __classToTypeDescription.put(Boolean.class, __boolean_TypeDescription);
        __classToTypeDescription.put(char.class, __char_TypeDescription);
        __classToTypeDescription.put(Character.class, __char_TypeDescription);
        __classToTypeDescription.put(byte.class, __byte_TypeDescription);
        __classToTypeDescription.put(Byte.class, __byte_TypeDescription);
        __classToTypeDescription.put(short.class, __short_TypeDescription);
        __classToTypeDescription.put(Short.class, __short_TypeDescription);
        __classToTypeDescription.put(int.class, __long_TypeDescription);
        __classToTypeDescription.put(Integer.class, __long_TypeDescription);
        __classToTypeDescription.put(long.class, __hyper_TypeDescription);
        __classToTypeDescription.put(Long.class, __hyper_TypeDescription);
        __classToTypeDescription.put(float.class, __float_TypeDescription);
        __classToTypeDescription.put(Float.class, __float_TypeDescription);
        __classToTypeDescription.put(double.class, __double_TypeDescription);
        __classToTypeDescription.put(Double.class, __double_TypeDescription);
        __classToTypeDescription.put(String.class, __string_TypeDescription);
        __classToTypeDescription.put(Type.class, __type_TypeDescription);
        __classToTypeDescription.put(Object.class, __any_TypeDescription);
        __classToTypeDescription.put(Any.class, __any_TypeDescription);
    }

    // must be sorted same as TypeClass:
    private static final TypeDescription[] __typeClassToTypeDescription
    = new TypeDescription[] {
        __void_TypeDescription,
        __char_TypeDescription,
        __boolean_TypeDescription,
        __byte_TypeDescription,
        __short_TypeDescription,
        __ushort_TypeDescription,
        __long_TypeDescription,
        __ulong_TypeDescription,
        __hyper_TypeDescription,
        __uhyper_TypeDescription,
        __float_TypeDescription,
        __double_TypeDescription,
        __string_TypeDescription,
        __type_TypeDescription,
        __any_TypeDescription
    };

    public static boolean isTypeClassSimple(TypeClass typeClass) {
        return typeClass.getValue() < __typeClassToTypeDescription.length;
    }

    static private TypeInfo []__getTypeInfos(Class zInterface) {
        TypeInfo typeInfos[] = null;

        try {
            Field field = zInterface.getField("UNOTYPEINFO");
            typeInfos = (TypeInfo[])field.get(null);

            if(typeInfos == null) { 
                // This is a workaround for a bug in solaris JDK 1.2, so please do not remove.
                // Interface members are sometimes not fully initialized,
                // reloading helps, who knows why?
                Class.forName(zInterface.getName());
                typeInfos = (TypeInfo[])field.get(null);
            }
        }
        catch(NoSuchFieldException noSuchFieldException) {
              if(DEBUG) System.err.println("__getTypeInfos:" + zInterface + " - exception:" + noSuchFieldException);
        }
        catch(ClassNotFoundException classNotFoundException) {
              if(DEBUG) System.err.println("__getTypeInfos:" + zInterface + " - exception:" + classNotFoundException);
        }
        catch(IllegalAccessException illegalAccessException) {
            System.err.println("__getTypeInfos:" + zInterface + " - exception:" + illegalAccessException);
        }
        
        return typeInfos;
    }

    /**
     * Find the member type info for the given interface and member name
     * <p>
     * This method does not belong to the provided <code>api</code>
     * <p>
     * @return the member type infos
     * @param  zInterface  the interface
     * @param  name        the member name
     * @see                com.sun.star.lib.uno.MemberTypeInfo
     */
    static private MemberTypeInfo __findMemberTypeInfo(TypeInfo typeInfos[], String name) {
        MemberTypeInfo memberTypeInfo = null;

        if(typeInfos != null) 
            for(int i = 0; i < typeInfos.length; ++ i) {
                if(typeInfos[i] instanceof MemberTypeInfo && typeInfos[i].getName().equals(name)) {
                    memberTypeInfo = (MemberTypeInfo)typeInfos[i];
                    break;
                }
            }

        return memberTypeInfo;
    }

    /**
     * Find the attribute type info for the given interface and attribute name
     * <p>
     * This method does not belong to the provided <code>api</code>
     * <p>
     * @return the attribute type infos
     * @param  zInterface  the interface
     * @param  name        the attribute name
     * @see                com.sun.star.lib.uno.AttributeTypeInfo
     */
    static private AttributeTypeInfo __findAttributeTypeInfo(TypeInfo typeInfos[], String name) {
        AttributeTypeInfo attributeTypeInfo = null;

        if(typeInfos != null) 
            for(int i = 0; i < typeInfos.length; ++ i) {
                if(typeInfos[i] instanceof AttributeTypeInfo && typeInfos[i].getName().equals(name)) {
                    attributeTypeInfo = (AttributeTypeInfo)typeInfos[i];
                    break;
                }
            }

        return attributeTypeInfo;
    }

    /**
     * Find the method type info for the given interface and method name.
     * <p>
     * This method does not belong to the provided <code>api</code>.
     * <p>
     * @return the method type infos
     * @param  zInterface  the interface
     * @param  methodName  the method name
     * @see                com.sun.star.lib.uno.MethodDescription
     */
    static private MethodTypeInfo __findMethodTypeInfo(TypeInfo typeInfos[], String methodName) {
        MethodTypeInfo methodTypeInfo = null;
        
        if(typeInfos != null) 
            for(int i = 0; i < typeInfos.length; ++ i) {
                if(typeInfos[i] instanceof MethodTypeInfo && typeInfos[i].getName().equals(methodName)) {
                    methodTypeInfo = (MethodTypeInfo)typeInfos[i];
                    break;
                }
            }

        return methodTypeInfo;
    }

    /**
     * Find the parameter type info for the given interface, method name and parameter index.
     * <p>
     * This method does not belong to the provided <code>api</code>.
     * <p>
     * @return the parameter type infos
     * @param  zInterface  the interface
     * @param  methodName  the method name
     * @param  index       the parameter index
     * @see                com.sun.star.lib.uno.ParameterTypeInfo
     */
    static private ParameterTypeInfo __findParameterTypeInfo(TypeInfo typeInfos[], String methodName, int index) { 
        ParameterTypeInfo parameterTypeInfo = null;

        if(typeInfos != null)
            for(int i = 0; i < typeInfos.length; ++ i) {
                if(typeInfos[i] instanceof ParameterTypeInfo 
                && (index == ((ParameterTypeInfo)typeInfos[i]).getIndex())
                && ((ParameterTypeInfo)typeInfos[i]).getMethodName().equals(methodName)) 
                {
                    parameterTypeInfo = (ParameterTypeInfo)typeInfos[i];
                    break;
                }
            }

        return parameterTypeInfo;
    }

    static private Hashtable __cyclicTypes = new Hashtable();

    static private Hashtable __typeCache_name = new Hashtable();
    static private Hashtable __typeCache_class = new Hashtable();
    static class CacheEntry {
        TypeDescription _typeDescription;

        short _index;
        short _prev;
        short _next;
        CacheEntry(TypeDescription typeDescription, short index, short prev, short next) {
            _typeDescription = typeDescription;
            _index = index;
            _prev = prev;
            _next = next;
        }
    }

    private static final boolean DEBUG_CACHE = false;
    private static CacheEntry __cacheEntrys[] = new CacheEntry[256];
    private static short __first;
    private static short __last = (short)(__cacheEntrys.length - 1);

    static {
        for(short i = 0; i < __cacheEntrys.length; ++ i)
            __cacheEntrys[i] = new CacheEntry(null, i, (short)(i - 1), (short)((i + 1) < __cacheEntrys.length ? i + 1 : -1));
    }

    static private void listCache() {
        synchronized(__cacheEntrys) {
            System.err.println("************ TypeDescription - Cache *************");
            short i = __first;
            while(i != (short)-1) {
                if(__cacheEntrys[i]._typeDescription != null)
                    System.err.println(__cacheEntrys[i]._typeDescription.getTypeName());
                else
                    System.err.println("null");

                i = __cacheEntrys[i]._next;
            }
        }
    }

    static private void touchCacheEntry(CacheEntry cacheEntry) {
          if(DEBUG_CACHE) System.err.println("touchCacheEntry:" + (cacheEntry._typeDescription != null ? cacheEntry._typeDescription.getTypeName() : null));

        if(cacheEntry != __cacheEntrys[__first]) {
            synchronized(__cacheEntrys) {
                if(DEBUG_CACHE) listCache();
                // move entry to front of chain

                // remove entry from chain
                __cacheEntrys[cacheEntry._prev]._next = cacheEntry._next;
                if(cacheEntry._next != -1) // not last entry
                    __cacheEntrys[cacheEntry._next]._prev = cacheEntry._prev;

                else
                    __last = cacheEntry._prev;

                // insert it as first element
                __cacheEntrys[__first]._prev = cacheEntry._index;
                cacheEntry._next = __first;
                
                __first = cacheEntry._index;

                if(DEBUG_CACHE) listCache();
            }
        }
    }
    
    static private TypeDescription getFromCache(String name) {
        TypeDescription typeDescription = null;

        CacheEntry cacheEntry = (CacheEntry)__typeCache_name.get(name);

          if(DEBUG_CACHE) System.err.println("getFromCache:" + name + " " + cacheEntry);

        if(cacheEntry != null) {
            touchCacheEntry(cacheEntry);
            
            typeDescription = cacheEntry._typeDescription;
        }

        return typeDescription;
    }

    static private TypeDescription getFromCache(Class zClass) {
        TypeDescription typeDescription = null;

        CacheEntry cacheEntry = (CacheEntry)__typeCache_class.get(zClass);

          if(DEBUG_CACHE) System.err.println("getFromCache:" + zClass + " " + cacheEntry);

        if(cacheEntry != null) {
            touchCacheEntry(cacheEntry);
            
            typeDescription = cacheEntry._typeDescription;
        }

        return typeDescription;
    }

    static private void addToCache(TypeDescription typeDescription) {
          if(DEBUG_CACHE) System.err.println("addToCache:" + typeDescription.getTypeName());

        // If typeDescription is a sequence type whose base component type is
        // any of SHORT, UNSIGNED SHORT, LONG, UNSIGNED LONG, HYPER, UNSIGNED
        // HYPER, ANY or com::sun::star::uno::XInterface, do not add it to the
        // cache.  Those UNO types have corresponding Java types that are not
        // unique (e.g., both a sequence of SHORT and a sequence of UNSIGNED
        // SHORT map to the Java type short[]), so using __typeCache_class to
        // map from such a Java type to a type description could give a wrong
        // result:
        String n = typeDescription.getTypeName();
        if (n.endsWith("[]short") || n.endsWith("[]unsigned short")
            || n.endsWith("[]long") || n.endsWith("[]unsigned long")
            || n.endsWith("[]hyper") || n.endsWith("[]unsigned hyper")
            || n.endsWith("[]any")
            || n.endsWith("[]com.sun.star.uno.XInterface")) {
            return;
        }

        synchronized(__cacheEntrys) {
              if(DEBUG_CACHE) listCache();

            // remove hashtab entry
            if(__cacheEntrys[__last]._typeDescription != null) {// maybe null, when cache not full
                __typeCache_name.remove(__cacheEntrys[__last]._typeDescription.getTypeName());
                __typeCache_class.remove(__cacheEntrys[__last]._typeDescription.getZClass());
            }

            // move last cache entry to front
            touchCacheEntry(__cacheEntrys[__last]);

            // replace typedescription with new typedescription
            __cacheEntrys[__first]._typeDescription = typeDescription;

            __typeCache_name.put(typeDescription.getTypeName(), __cacheEntrys[__first]);
            __typeCache_class.put(typeDescription.getZClass(), __cacheEntrys[__first]);

            if(DEBUG_CACHE) listCache();
        }
    }

    static public TypeDescription getTypeDescription(TypeInfo typeInfo, Class zClass) {
        if(DEBUG) System.err.println("TypeDescription.getTypeDescription:" + typeInfo + " " + zClass);

          TypeDescription typeDescription = null;

        if(typeInfo == null || !typeInfo.isInterface())
            typeDescription = TypeDescription.getTypeDescription(zClass);

        else {
            // blackdown 118 bug workaround
//  				__getFields(zClass);
            
            // see if the typeinfo says, that the parameter is an interface
            // and the parameter is not assignable to xinterface (mapping of xinterface to java.lang.Object)
            // set the parameter to class of xinterface, cause it must be assignable

            // unrole any arrays
            int arrayNesting = 0;
            Class xClass = zClass;
            while(xClass.isArray()) {
                ++ arrayNesting;
                
                xClass = xClass.getComponentType();
            }

            if(xClass.isInterface()) 
                typeDescription = TypeDescription.getTypeDescription(zClass);

            else {
                typeDescription = TypeDescription.getTypeDescription(com.sun.star.uno.XInterface.class);

                for(int i = 0; i < arrayNesting; ++ i) {
                    try {
                        typeDescription = TypeDescription.getTypeDescription("[]" + typeDescription.getTypeName());
                    }
                    catch(ClassNotFoundException classNotFoundException) { // this could never happen, but anyway...
                        throw new RuntimeException(classNotFoundException.toString());
                    }
                }
            }
          }


        return typeDescription;
    }

    static public TypeDescription getTypeDescription(Class zClass) {
        TypeDescription typeDescription = (TypeDescription)__classToTypeDescription.get(zClass);

        if(typeDescription == null) {
            typeDescription = (TypeDescription)__cyclicTypes.get(zClass.getName());

            if(typeDescription == null) {
                typeDescription = getFromCache(zClass);

                if(typeDescription == null) {
                    typeDescription = new TypeDescription(zClass);

                    addToCache(typeDescription);
                }
            }
        }

        return typeDescription;
    }

    static public TypeDescription getTypeDescription(String typeName) throws ClassNotFoundException {
        TypeDescription typeDescription = (TypeDescription)__typeNameToTypeDescription.get(typeName);

        if(typeDescription == null) {
            typeDescription = (TypeDescription)__cyclicTypes.get(typeName);

            if(typeDescription == null) {
                typeDescription = getFromCache(typeName);

                if(typeDescription == null) {
                    typeDescription = new TypeDescription(typeName);

                    addToCache(typeDescription);
                }
            }
        }


        return typeDescription;
    }

    public static ITypeDescription getTypeDescription(Type type)
        throws ClassNotFoundException
    {
        ITypeDescription td = type.getTypeDescription();
        if (td == null) {
            if (type.getZClass() != null) {
                td = getTypeDescription(type.getZClass());
            } else {
                td = getTypeDescription(type.getTypeClass());
                if (td == null) {
                    td = getTypeDescription(type.getTypeName());
                }
            }
            type.setTypeDescription(td);
        }
        return td;
    }

    public static TypeDescription getTypeDescription(TypeClass typeClass) {
        int n = typeClass.getValue();
        return n < __typeClassToTypeDescription.length
            ? __typeClassToTypeDescription[n] : null;
    }

    protected TypeClass _typeClass;
    protected String    _typeName;
    protected String    _arrayTypeName;
    protected Class     _class;

    protected TypeDescription _superType; // if this is a struct or an interface

    protected int               _offset;
    protected MethodDescription _methodDescriptions[];
    protected Hashtable         _methodDescriptionsByName;

    protected Hashtable         _iFieldDescriptionsByName;
    protected IFieldDescription _iFieldDescriptions[];

    protected ITypeDescription _componentType;


    private int calcMethodOffset(Class zClass) {
        int offset = 3;

        if(zClass != com.sun.star.uno.XInterface.class) {
            Method methods[] = zClass.getMethods();
            
            for(int i = 0; i < methods.length; ++ i)
                if((methods[i].getModifiers() & Modifier.STATIC) == 0)
                    ++ offset;
        }

        return offset;
    }

    private void _initByClass(Class zClass) {
        __cyclicTypes.put(zClass.getName(), this);

        _class = zClass;

        if(Enum.class.isAssignableFrom(zClass)) {
            _typeClass = TypeClass.ENUM;
            _typeName  = zClass.getName();

            _arrayTypeName = "[L" + _class.getName() + ";";
        }
        else if(Union.class.isAssignableFrom(zClass)) {
            _typeClass = TypeClass.UNION;
            _typeName  = zClass.getName();

            _arrayTypeName = "[L" + _class.getName() + ";";
        }
        else if(zClass.isInterface()) {
            _typeClass = TypeClass.INTERFACE;
            _typeName  = zClass.getName();

            _arrayTypeName = "[L" + _class.getName() + ";";

            _offset = calcMethodOffset(zClass);

            Class interfaces[] = _class.getInterfaces();
            if(interfaces.length > 0)
                _superType = getTypeDescription(interfaces[0]); // uno only supports single inheritance
        }
        else if(zClass.isArray()) {
            _typeClass = TypeClass.SEQUENCE;
            
            _componentType = getTypeDescription(zClass.getComponentType());
            _typeName = "[]" + _componentType.getTypeName();

            _arrayTypeName = "[" + _class.getName();
        }
        else if(java.lang.Throwable.class.isAssignableFrom(zClass)) {
            _typeClass = TypeClass.EXCEPTION;
            _typeName  = zClass.getName();

            _arrayTypeName = "[L" + _class.getName() + ";";

            Class superClass = _class.getSuperclass();
            if(superClass != null && superClass != Object.class)
                _superType = getTypeDescription(superClass);
        }
        else {
            _typeClass = TypeClass.STRUCT;
            _typeName  = zClass.getName();

            _arrayTypeName = "[L" + _class.getName() + ";";

            Class superClass = _class.getSuperclass();
            if(superClass != null && superClass != Object.class)
                _superType = getTypeDescription(superClass);
        }

          __cyclicTypes.remove(zClass.getName());
    }

    private TypeDescription(TypeClass typeClass, String typeName, String arrayTypeName, Class zClass) {
        _typeClass     = typeClass;
        _typeName      = typeName;
        _arrayTypeName = arrayTypeName;
        _class         = zClass;
        // _componentType == null is ok, since only primitive TypeDescriptions
        // are created with this constructor
    }

    private TypeDescription(Class zClass) {
        _initByClass(zClass);
    }

    private TypeDescription(String typeName) throws ClassNotFoundException {

        _typeName = typeName;

        if(typeName.startsWith("[]")) {
            __cyclicTypes.put(typeName, this);
            _typeClass = TypeClass.SEQUENCE;

            _componentType = getTypeDescription(typeName.substring(2));
            _class = Class.forName(_componentType.getArrayTypeName());
            _arrayTypeName = "[" + _componentType.getArrayTypeName();

            __cyclicTypes.remove(typeName);
        }
        else {
            _initByClass(Class.forName(typeName));
        }

    }

    private void _initMethodTypeInfos() {
        if(DEBUG) System.err.println("TypeDescription._initMethodTypeInfos");

        Hashtable methodDescriptionsByName = new Hashtable();

        int superOffset = 0;

        if(_class == com.sun.star.uno.XInterface.class) { // take special care for xinterface
              MethodDescription queryInterface = new MethodDescription("queryInterface", 0, TypeInfo.ANY);
            queryInterface.init(new Class[]{Type.class}, new ParameterTypeInfo[]{new ParameterTypeInfo(null, "queryInterface", 0, 0)},
                                Object.class);

            MethodDescription acquire = new MethodDescription("acquire", 1, TypeInfo.ONEWAY);
            acquire.init(new Class[0], null, Void.class);

            MethodDescription release = new MethodDescription("release", 2, TypeInfo.ONEWAY);
            release.init(new Class[0], null, Void.class);

            methodDescriptionsByName.put("queryInterface", queryInterface);
            methodDescriptionsByName.put("acquire", acquire);
            methodDescriptionsByName.put("release", release);

            _offset = 3;
        }
        else {
            if(_superType != null) // do we have a supertype (we don't have one, if we are not derived of XInterface)
                superOffset = _superType._offset;

            TypeInfo typeInfos[] = __getTypeInfos(_class);
            Method methods[] = _class.getMethods();
            
            for(int i = 0; i < methods.length; ++ i) {
                Class parameters[] = methods[i].getParameterTypes();
                ParameterTypeInfo[] parameterTypeInfos = null;
                String name = methods[i].getName();
                MethodTypeInfo methodTypeInfo = __findMethodTypeInfo(typeInfos,
                                                                     name);
                MethodDescription methodDescription = null;

                if(methodTypeInfo != null) { // this is a idl method
                    methodDescription = new MethodDescription(methodTypeInfo);
                    parameterTypeInfos = new ParameterTypeInfo[
                        parameters.length];
                    for(int j = 0; j < parameters.length; ++ j)
                        parameterTypeInfos[j] = __findParameterTypeInfo(
                            typeInfos, name, j);
                } else {
                    final int NO_ATTRIBUTE = 0;
                    final int GET_ATTRIBUTE = 1;
                    final int SET_ATTRIBUTE = 2;
                    int attr = name.startsWith("get") ? GET_ATTRIBUTE
                        : name.startsWith("set") ? SET_ATTRIBUTE
                        : NO_ATTRIBUTE;
                    if (attr != NO_ATTRIBUTE) {
                        AttributeTypeInfo ati = __findAttributeTypeInfo(
                            typeInfos, name.substring("?et".length()));
                        if (ati != null) {
                            if (attr == GET_ATTRIBUTE) {
                                methodDescription = new MethodDescription(
                                    name, ati.getIndex(), ati.getFlags());
                            } else {
                                methodDescription = new MethodDescription(
                                    name, ati.getIndex() + 1, 0);
                                parameterTypeInfos = new ParameterTypeInfo[] {
                                    new ParameterTypeInfo("", name, 0,
                                                          ati.getFlags()) };
                            }
                        }
                    }
                }
                
                if(methodDescription != null) {
                    methodDescription.init(methods[i], parameterTypeInfos, superOffset);

                    methodDescriptionsByName.put(methodDescription.getName(), methodDescription);
                }
            }
            
        }

        MethodDescription methodDescriptions[] = new MethodDescription[methodDescriptionsByName.size()];
        Enumeration element = methodDescriptionsByName.elements();
        while(element.hasMoreElements()) {
            MethodDescription methodDescription = (MethodDescription)element.nextElement();
            
            methodDescriptions[methodDescription.getIndex() - superOffset] = methodDescription;
        }

        _offset = superOffset + methodDescriptions.length;

        // transactional assignment 
        _methodDescriptions       = methodDescriptions;
        _methodDescriptionsByName = methodDescriptionsByName;
    }
    
    private void _initMemberTypeInfos() {
        if(DEBUG) System.err.println("TypeDescription._initMemberTypeInfos");

        TypeInfo typeInfos[] = __getTypeInfos(_class);
        Field fields[] = _class.getFields();
        int index = 0;
        int superTypeMemberCount = 0;
        if( _superType != null )
            superTypeMemberCount = _superType.getFieldDescriptions().length;
        
        IFieldDescription tmp_iFieldDescriptions[] = new IFieldDescription[fields.length];

        Hashtable iFieldDescriptionsByName = new Hashtable();

        int i;
        for( i = 0; i < fields.length; ++ i) {
            if((fields[i].getModifiers() & (Modifier.STATIC | Modifier.TRANSIENT)) == 0) { // neither static nor transient ?

                IFieldDescription iFieldDescription = null;
                
                if(_superType != null)
                    iFieldDescription = _superType.getFieldDescription(fields[i].getName());
                
                if(iFieldDescription == null) {
                    MemberTypeInfo memberTypeInfo = __findMemberTypeInfo(typeInfos, fields[i].getName());

                    if( memberTypeInfo == null )
                        throw new com.sun.star.uno.RuntimeException(
                            TypeDescription.class.getName() + "no membertype info for field " + fields[i].getName(),null );
                    
                    iFieldDescription = new FieldDescription(
                        memberTypeInfo.getName(),
                        memberTypeInfo.getIndex() + superTypeMemberCount,
                        memberTypeInfo.getFlags(),
                        fields[i]);
                }
                
                iFieldDescriptionsByName.put(iFieldDescription.getName(), iFieldDescription);
                tmp_iFieldDescriptions[index ++] = iFieldDescription;
            }
        }

        IFieldDescription iFieldDescriptions[] = new IFieldDescription[index];
        for( i = 0; i < index ; i ++ )
        {
            iFieldDescriptions[tmp_iFieldDescriptions[i].getIndex()] = tmp_iFieldDescriptions[i];
        }

        // transactional assignment 
        _iFieldDescriptions = iFieldDescriptions;
        _iFieldDescriptionsByName = iFieldDescriptionsByName;
    }

    public ITypeDescription getSuperType() {
        return _superType;
    }

    public IMethodDescription []getMethodDescriptions() {
        if(_methodDescriptions == null)
            _initMethodTypeInfos();

        return _methodDescriptions;
    }

    public IMethodDescription getMethodDescription(int methodId) {
        if(_methodDescriptionsByName == null)
            _initMethodTypeInfos();

        IMethodDescription iMethodDescription = null;

        int relMethodId = methodId - (_offset - _methodDescriptions.length);

        if(relMethodId >= 0 && relMethodId < _methodDescriptions.length)
            iMethodDescription = _methodDescriptions[relMethodId];

        else if(_superType != null)
            iMethodDescription = _superType.getMethodDescription(methodId);

        return iMethodDescription;
    }

    public IMethodDescription getMethodDescription(String name) {
        if(_methodDescriptionsByName == null)
            _initMethodTypeInfos();

        IMethodDescription iMethodDescription = (MethodDescription)_methodDescriptionsByName.get(name);
        if(iMethodDescription == null && _superType != null)
            iMethodDescription = _superType.getMethodDescription(name);

        return iMethodDescription;
    }

    public IFieldDescription []getFieldDescriptions() {
        if(_iFieldDescriptions == null)
            _initMemberTypeInfos();

        return _iFieldDescriptions;
    }

    public IFieldDescription getFieldDescription(String name) {
        if(_iFieldDescriptions == null)
            _initMemberTypeInfos();

        IFieldDescription iFieldDescription = (IFieldDescription)_iFieldDescriptionsByName.get(name);
        if(iFieldDescription == null && _superType != null)
            iFieldDescription = _superType.getFieldDescription(name);

        return iFieldDescription;
    }

    /**
     * Gets the IDL <code>TypeClass</code> of the type.
     * <p>
     * @return  the <code>TypeClass</code>.
     */
    public TypeClass getTypeClass() {
        return _typeClass;
    }

    /**
     * Gets the component <code>TypeDescription</code> if this is a sequence
     * type.
     *
     * @return the component <code>TypeDescription</code>, or <code>null</code>
     *     if this is not a sequence type
     */
    public ITypeDescription getComponentType() {
        return _componentType;
    }

    /**
     * Gets the type name.
     * <p>
     * @return  the type name.
     */
    public String getTypeName() { 
        return _typeName;
    }
    
    /**
     * Gets the array type name.
     * <p>
     * @return  the array type name.
     */
    public String getArrayTypeName() { 
        return _arrayTypeName;
    }
    
    /**
     * Gets the corresponding java class for the type.
     * <p>
     * @return   the corresponding java class.
     */
    public Class getZClass() { 
        return _class;
    }


    public boolean equals(Object typeDescription) {
        boolean result = false;

        if(typeDescription != null) 
            result = _typeName.equals(((TypeDescription)typeDescription)._typeName);

        return result;
    }

    public String toString() {
        return "<" + getTypeName() + ">";
    }
}

