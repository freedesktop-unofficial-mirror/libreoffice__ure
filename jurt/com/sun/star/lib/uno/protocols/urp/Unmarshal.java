/*************************************************************************
 *
 *  $RCSfile: Unmarshal.java,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: obo $ $Date: 2004-06-03 14:35:35 $
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
package com.sun.star.lib.uno.protocols.urp;

import com.sun.star.lib.uno.environments.remote.ThreadId;
import com.sun.star.lib.uno.typedesc.FieldDescription;
import com.sun.star.lib.uno.typedesc.TypeDescription;
import com.sun.star.uno.Any;
import com.sun.star.uno.Enum;
import com.sun.star.uno.IBridge;
import com.sun.star.uno.IFieldDescription;
import com.sun.star.uno.Type;
import com.sun.star.uno.TypeClass;
import com.sun.star.uno.XInterface;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.lang.reflect.Array;
import java.lang.reflect.InvocationTargetException;

final class Unmarshal {
    public Unmarshal(IBridge bridge, int cacheSize) {
        this.bridge = bridge;
        objectIdCache = new String[cacheSize];
        threadIdCache = new ThreadId[cacheSize];
        typeCache = new TypeDescription[cacheSize];
        reset(new byte[0]);
    }

    public int read8Bit() {
        try {
            return input.readUnsignedByte();
        } catch (IOException e) {
            throw new RuntimeException(e.toString());
        }
    }

    public int read16Bit() {
        try {
            return input.readUnsignedShort();
        } catch (IOException e) {
            throw new RuntimeException(e.toString());
        }
    }

    public String readObjectId() {
        String id = readStringValue();
        int index = read16Bit();
        if (index == 0xFFFF) {
            if (id.length() == 0) {
                id = null;
            }
        } else {
            if (id.length() == 0) {
                id = objectIdCache[index];
            } else {
                objectIdCache[index] = id;
            }
        }
        return id;
    }

    public ThreadId readThreadId() {
        int len = readCompressedNumber();
        byte[] data = null;
        ThreadId id = null;
        if (len != 0) {
            data = new byte[len];
            readBytes(data);
            id = new ThreadId(data);
        }
        int index = read16Bit();
        if (index != 0xFFFF) {
            if (len == 0) {
                id = threadIdCache[index];
            } else {
                threadIdCache[index] = id;
            }
        }
        return id;
    }

    public TypeDescription readType() {
        int b = read8Bit();
        TypeClass typeClass = TypeClass.fromInt(b & 0x7F);
        if (TypeDescription.isTypeClassSimple(typeClass)) {
            return TypeDescription.getTypeDescription(typeClass);
        } else {
            int index = read16Bit();
            TypeDescription type = null;
            if ((b & 0x80) != 0) {
                try {
                    type = TypeDescription.getTypeDescription(
                        readStringValue());
                } catch (ClassNotFoundException e) {
                    throw new RuntimeException(e.toString());
                }
            }
            if (index != 0xFFFF) {
                if ((b & 0x80) == 0) {
                    type = typeCache[index];
                } else {
                    typeCache[index] = type;
                }
            }
            return type;
        }
    }

    public Object readValue(TypeDescription type) {
        switch (type.getTypeClass().getValue()) {
        case TypeClass.VOID_value:
            return null;

        case TypeClass.BOOLEAN_value:
            return readBooleanValue();

        case TypeClass.BYTE_value:
            return readByteValue();

        case TypeClass.SHORT_value:
        case TypeClass.UNSIGNED_SHORT_value:
            return readShortValue();

        case TypeClass.LONG_value:
        case TypeClass.UNSIGNED_LONG_value:
            return readLongValue();

        case TypeClass.HYPER_value:
        case TypeClass.UNSIGNED_HYPER_value:
            return readHyperValue();

        case TypeClass.FLOAT_value:
            return readFloatValue();

        case TypeClass.DOUBLE_value:
            return readDoubleValue();

        case TypeClass.CHAR_value:
            return readCharValue();

        case TypeClass.STRING_value:
            return readStringValue();

        case TypeClass.TYPE_value:
            return readTypeValue();

        case TypeClass.ANY_value:
            return readAnyValue();

        case TypeClass.SEQUENCE_value:
            return readSequenceValue(type);

        case TypeClass.ENUM_value:
            return readEnumValue(type);

        case TypeClass.STRUCT_value:
            return readStructValue(type);

        case TypeClass.EXCEPTION_value:
            return readExceptionValue(type);

        case TypeClass.INTERFACE_value:
            return readInterfaceValue(type);

        default:
            throw new IllegalArgumentException("Bad type descriptor " + type);
        }
    }

    public boolean hasMore() {
        try {
            return input.available() > 0;
        } catch (IOException e) {
            throw new RuntimeException(e.toString());
        }
    }

    public void reset(byte[] data) {
        input = new DataInputStream(new ByteArrayInputStream(data));
    }

    private Boolean readBooleanValue() {
        try {
            return input.readBoolean() ? Boolean.TRUE : Boolean.FALSE;
        } catch (IOException e) {
            throw new RuntimeException(e.toString());
        }
    }

    private Byte readByteValue() {
        try {
            return new Byte(input.readByte());
        } catch (IOException e) {
            throw new RuntimeException(e.toString());
        }
    }

    private Short readShortValue() {
        try {
            return new Short(input.readShort());
        } catch (IOException e) {
            throw new RuntimeException(e.toString());
        }
    }

    private Integer readLongValue() {
        try {
            return new Integer(input.readInt());
        } catch (IOException e) {
            throw new RuntimeException(e.toString());
        }
    }

    private Long readHyperValue() {
        try {
            return new Long(input.readLong());
        } catch (IOException e) {
            throw new RuntimeException(e.toString());
        }
    }

    private Float readFloatValue() {
        try {
            return new Float(input.readFloat());
        } catch (IOException e) {
            throw new RuntimeException(e.toString());
        }
    }

    private Double readDoubleValue() {
        try {
            return new Double(input.readDouble());
        } catch (IOException e) {
            throw new RuntimeException(e.toString());
        }
    }

    private Character readCharValue() {
        try {
            return new Character(input.readChar());
        } catch (IOException e) {
            throw new RuntimeException(e.toString());
        }
    }

    private String readStringValue() {
        int len = readCompressedNumber();
        byte[] data = new byte[len];
        readBytes(data);
        try {
            return new String(data, "UTF8");
        } catch (UnsupportedEncodingException e) {
            throw new RuntimeException(e.toString());
        }
    }

    private Type readTypeValue() {
        return new Type(readType());
    }

    private Object readAnyValue() {
        TypeDescription type = readType();
        switch (type.getTypeClass().getValue()) {
        case TypeClass.VOID_value:
            return Any.VOID;

        case TypeClass.BOOLEAN_value:
            return readBooleanValue();

        case TypeClass.BYTE_value:
            return readByteValue();

        case TypeClass.SHORT_value:
            return readShortValue();

        case TypeClass.UNSIGNED_SHORT_value:
            return new Any(Type.UNSIGNED_SHORT, readShortValue());

        case TypeClass.LONG_value:
            return readLongValue();

        case TypeClass.UNSIGNED_LONG_value:
            return new Any(Type.UNSIGNED_LONG, readLongValue());

        case TypeClass.HYPER_value:
            return readHyperValue();

        case TypeClass.UNSIGNED_HYPER_value:
            return new Any(Type.UNSIGNED_HYPER, readHyperValue());

        case TypeClass.FLOAT_value:
            return readFloatValue();

        case TypeClass.DOUBLE_value:
            return readDoubleValue();

        case TypeClass.CHAR_value:
            return readCharValue();

        case TypeClass.STRING_value:
            return readStringValue();

        case TypeClass.TYPE_value:
            return readTypeValue();

        case TypeClass.SEQUENCE_value:
            {
                Object value = readSequenceValue(type);
                TypeDescription ctype = (TypeDescription)
                    type.getComponentType();
                while (ctype.getTypeClass() == TypeClass.SEQUENCE) {
                    ctype = (TypeDescription) ctype.getComponentType();
                }
                switch (ctype.getTypeClass().getValue()) {
                case TypeClass.UNSIGNED_SHORT_value:
                case TypeClass.UNSIGNED_LONG_value:
                case TypeClass.UNSIGNED_HYPER_value:
                    return new Any(new Type(type), value);

                case TypeClass.STRUCT_value:
                    if (ctype.hasTypeArguments()) {
                        return new Any(new Type(type), value);
                    }
                default:
                    return value;
                }
            }

        case TypeClass.ENUM_value:
            return readEnumValue(type);

        case TypeClass.STRUCT_value:
            {
                Object value = readStructValue(type);
                return type.hasTypeArguments()
                    ? new Any(new Type(type), value) : value;
            }

        case TypeClass.EXCEPTION_value:
            return readExceptionValue(type);

        case TypeClass.INTERFACE_value:
            {
                Object value = readInterfaceValue(type);
                return type.getZClass() == XInterface.class
                    ? value : new Any(new Type(type), value);
            }

        default:
            throw new RuntimeException(
                "Reading ANY with bad type " + type.getTypeClass());
        }
    }

    private Object readSequenceValue(TypeDescription type) {
        int len = readCompressedNumber();
        if (type.getTypeClass() == TypeClass.BYTE) {
            byte[] data = new byte[len];
            readBytes(data);
            return data;
        } else {
            TypeDescription ctype = (TypeDescription) type.getComponentType();
            Object value = Array.newInstance(
                ctype.getTypeClass() == TypeClass.ANY
                ? Object.class : ctype.getZClass(), len);
            for (int i = 0; i < len; ++i) {
                Array.set(value, i, readValue(ctype));
            }
            return value;
        }
    }

    private Enum readEnumValue(TypeDescription type) {
        try {
            return (Enum)
                type.getZClass().getMethod(
                    "fromInt", new Class[] { int.class }).
                invoke(null, new Object[] { readLongValue() });
        } catch (IllegalAccessException e) {
            throw new RuntimeException(e.toString());
        } catch (InvocationTargetException e) {
            throw new RuntimeException(e.toString());
        } catch (NoSuchMethodException e) {
            throw new RuntimeException(e.toString());
        }
    }

    private Object readStructValue(TypeDescription type) {
        Object value;
        try {
            value = type.getZClass().newInstance();
        } catch (IllegalAccessException e) {
            throw new RuntimeException(e.toString());
        } catch (InstantiationException e) {
            throw new RuntimeException(e.toString());
        }
        readFields(type, value);
        return value;
    }

    private Exception readExceptionValue(TypeDescription type) {
        Exception value;
        try {
            value = (Exception)
                type.getZClass().getConstructor(new Class[] { String.class }).
                newInstance(new Object[] { readStringValue() });
        } catch (IllegalAccessException e) {
            throw new RuntimeException(e.toString());
        } catch (InstantiationException e) {
            throw new RuntimeException(e.toString());
        } catch (InvocationTargetException e) {
            throw new RuntimeException(e.toString());
        } catch (NoSuchMethodException e) {
            throw new RuntimeException(e.toString());
        }
        readFields(type, value);
        return value;
    }

    private Object readInterfaceValue(TypeDescription type) {
        String id = readObjectId();
        return id == null ? null : bridge.mapInterfaceFrom(id, new Type(type));
    }

    private int readCompressedNumber() {
        int number = read8Bit();
        try {
            return number < 0xFF ? number : input.readInt();
        } catch (IOException e) {
            throw new RuntimeException(e.toString());
        }
    }

    private void readBytes(byte[] data) {
        try {
            input.readFully(data);
        } catch (IOException e) {
            throw new RuntimeException(e.toString());
        }
    }

    private void readFields(TypeDescription type, Object value) {
        IFieldDescription[] fields = type.getFieldDescriptions();
        for (int i = 0; i < fields.length; ++i) {
            int index = ((FieldDescription) fields[i]).getTypeParameterIndex();
            try {
                fields[i].getField().set(
                    value,
                    readValue(
                        index < 0
                        ? (TypeDescription) fields[i].getTypeDescription()
                        : type.getTypeArgument(index)));
            } catch (IllegalAccessException e) {
                throw new RuntimeException(e.toString());
            }
        }
    }

    private final IBridge bridge;
    private final String[] objectIdCache;
    private final ThreadId[] threadIdCache;
    private final TypeDescription[] typeCache;
    private DataInputStream input;
}
