/*************************************************************************
 *
 *  $RCSfile: Marshal.java,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: obo $ $Date: 2004-06-03 14:35:15 $
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
import java.io.ByteArrayOutputStream;
import java.io.DataOutput;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.lang.reflect.Array;
import java.lang.reflect.InvocationTargetException;

final class Marshal {
    public Marshal(IBridge bridge, short cacheSize) {
        this.bridge = bridge;
        objectIdCache = new Cache(cacheSize);
        threadIdCache = new Cache(cacheSize);
        typeCache = new Cache(cacheSize);
    }

    public void write8Bit(int value) {
        try {
            output.writeByte(value);
        } catch (IOException e) {
            throw new RuntimeException(e.toString());
        }
    }

    public void write16Bit(int value) {
        try {
            output.writeShort(value);
        } catch (IOException e) {
            throw new RuntimeException(e.toString());
        }
    }

    public void writeObjectId(String objectId) {
        if (objectId == null) {
            writeStringValue(null);
            write16Bit(0xFFFF);
        } else {
            boolean[] found = new boolean[1];
            short index = objectIdCache.add(found, objectId);
            writeStringValue(found[0] ? null : objectId);
            write16Bit(index);
        }
    }

    public void writeThreadId(ThreadId threadId) {
        byte[] data = threadId.getBytes();
        boolean[] found = new boolean[1];
        short index = threadIdCache.add(found, data);
        if (found[0]) {
            writeCompressedNumber(0);
        } else {
            writeCompressedNumber(data.length);
            writeBytes(data);
        }
        write16Bit(index);
    }

    public void writeType(TypeDescription type) {
        TypeClass typeClass = type.getTypeClass();
        if (TypeDescription.isTypeClassSimple(typeClass)) {
            write8Bit(typeClass.getValue());
        } else {
            boolean[] found = new boolean[1];
            short index = typeCache.add(found, type.getTypeName());
            write8Bit(typeClass.getValue() | (found[0] ? 0 : 0x80));
            write16Bit(index);
            if (!found[0]) {
                writeStringValue(type.getTypeName());
            }
        }
    }

    public void writeValue(TypeDescription type, Object value) {
        switch(type.getTypeClass().getValue()) {
        case TypeClass.VOID_value:
            break;

        case TypeClass.BOOLEAN_value:
            writeBooleanValue((Boolean) value);
            break;

        case TypeClass.BYTE_value:
            writeByteValue((Byte) value);
            break;

        case TypeClass.SHORT_value:
        case TypeClass.UNSIGNED_SHORT_value:
            writeShortValue((Short) value);
            break;

        case TypeClass.LONG_value:
        case TypeClass.UNSIGNED_LONG_value:
            writeLongValue((Integer) value);
            break;

        case TypeClass.HYPER_value:
        case TypeClass.UNSIGNED_HYPER_value:
            writeHyperValue((Long) value);
            break;

        case TypeClass.FLOAT_value:
            writeFloatValue((Float) value);
            break;

        case TypeClass.DOUBLE_value:
            writeDoubleValue((Double) value);
            break;

        case TypeClass.CHAR_value:
            writeCharValue((Character) value);
            break;

        case TypeClass.STRING_value:
            writeStringValue((String) value);
            break;

        case TypeClass.TYPE_value:
            writeTypeValue((Type) value);
            break;

        case TypeClass.ANY_value:
            writeAnyValue(value);
            break;

        case TypeClass.SEQUENCE_value:
            writeSequenceValue(type, value);
            break;

        case TypeClass.ENUM_value:
            writeEnumValue(type, (Enum) value);
            break;

        case TypeClass.STRUCT_value:
            writeStructValue(type, value);
            break;

        case TypeClass.EXCEPTION_value:
            writeExceptionValue(type, (Exception) value);
            break;

        case TypeClass.INTERFACE_value:
            writeInterfaceValue(type, (XInterface) value);
            break;

        default:
            throw new IllegalArgumentException("Bad type descriptor " + type);
        }
    }

    public byte[] reset() {
        byte[] data = buffer.toByteArray();
        buffer.reset();
        return data;
    }

    private void writeBooleanValue(Boolean value) {
        try {
            output.writeBoolean(value != null && value.booleanValue());
        } catch (IOException e) {
            throw new RuntimeException(e.toString());
        }
    }

    private void writeByteValue(Byte value) {
        write8Bit(value == null ? 0 : value.byteValue());
    }

    private void writeShortValue(Short value) {
        write16Bit(value == null ? 0 : value.shortValue());
    }

    private void writeLongValue(Integer value) {
        write32Bit(value == null ? 0 : value.intValue());
    }

    private void writeHyperValue(Long value) {
        try {
            output.writeLong(value == null ? 0 : value.longValue());
        } catch (IOException e) {
            throw new RuntimeException(e.toString());
        }
    }

    private void writeFloatValue(Float value) {
        try {
            output.writeFloat(value == null ? 0 : value.floatValue());
        } catch (IOException e) {
            throw new RuntimeException(e.toString());
        }
    }

    private void writeDoubleValue(Double value) {
        try {
            output.writeDouble(value == null ? 0 : value.doubleValue());
        } catch (IOException e) {
            throw new RuntimeException(e.toString());
        }
    }

    private void writeCharValue(Character value) {
        try {
            output.writeChar(value == null ? 0 : value.charValue());
        } catch (IOException e) {
            throw new RuntimeException(e.toString());
        }
    }

    private void writeStringValue(String value) {
        if (value == null) {
            writeCompressedNumber(0);
        } else {
            byte[] data;
            try {
                data = value.getBytes("UTF8");
            } catch (UnsupportedEncodingException e) {
                throw new RuntimeException(e.toString());
            }
            writeCompressedNumber(data.length);
            writeBytes(data);
        }
    }

    private void writeTypeValue(Type value) {
        try {
            writeType(
                TypeDescription.getTypeDescription(
                    value == null ? Type.VOID : value));
        } catch (ClassNotFoundException e) {
            throw new RuntimeException(e.toString());
        }
    }

    private void writeAnyValue(Object value) {
        TypeDescription type;
        if (value == null || value instanceof XInterface) {
            type = TypeDescription.getTypeDescription(XInterface.class);
        } else if (value instanceof Any) {
            Any any = (Any) value;
            try {
                type = TypeDescription.getTypeDescription(any.getType());
            } catch (ClassNotFoundException e) {
                throw new RuntimeException(e.toString());
            }
            value = any.getObject();
        } else {
            type = TypeDescription.getTypeDescription(value.getClass());
        }
        writeType(type);
        writeValue(type, value);
    }

    private void writeSequenceValue(TypeDescription type, Object value) {
        if (value == null) {
            writeCompressedNumber(0);
        } else {
            TypeDescription ctype = (TypeDescription) type.getComponentType();
            if (ctype.getTypeClass() == TypeClass.BYTE) {
                byte[] data = (byte[]) value;
                writeCompressedNumber(data.length);
                writeBytes(data);
            } else {
                int len = Array.getLength(value);
                writeCompressedNumber(len);
                for (int i = 0; i < len; ++i) {
                    writeValue(ctype, Array.get(value, i));
                }
            }
        }
    }

    private void writeEnumValue(TypeDescription type, Enum value) {
        int n;
        if (value == null) {
            try {
                n = ((Enum)
                     (type.getZClass().getMethod("getDefault", null).
                      invoke(null, null))).
                    getValue();
            } catch (IllegalAccessException e) {
                throw new RuntimeException(e.toString());
            } catch (InvocationTargetException e) {
                throw new RuntimeException(e.toString());
            } catch (NoSuchMethodException e) {
                throw new RuntimeException(e.toString());
            }
        } else {
            n = value.getValue();
        }
        write32Bit(n);
    }

    private void writeStructValue(TypeDescription type, Object value) {
        IFieldDescription[] fields = type.getFieldDescriptions();
        for (int i = 0; i < fields.length; ++i) {
            int index = ((FieldDescription) fields[i]).getTypeParameterIndex();
            try {
                writeValue(
                    (index < 0
                     ? (TypeDescription) fields[i].getTypeDescription()
                     : type.getTypeArgument(index)),
                    value == null ? null : fields[i].getField().get(value));
            } catch (IllegalAccessException e) {
                throw new RuntimeException(e.toString());
            }
        }
    }

    private void writeExceptionValue(TypeDescription type, Exception value) {
        writeStringValue(value == null ? null : value.getMessage());
        writeStructValue(type, value);
    }

    private void writeInterfaceValue(TypeDescription type, XInterface value) {
        writeObjectId((String) bridge.mapInterfaceTo(value, new Type(type)));
    }

    private void write32Bit(int value) {
        try {
            output.writeInt(value);
        } catch (IOException e) {
            throw new RuntimeException(e.toString());
        }
    }

    private void writeCompressedNumber(int number) {
        if (number >= 0 && number < 0xFF) {
            write8Bit(number);
        } else {
            write8Bit(0xFF);
            write32Bit(number);
        }
    }

    private void writeBytes(byte[] data) {
        try {
            output.write(data);
        } catch (IOException e) {
            throw new RuntimeException(e.toString());
        }
    }

    private final ByteArrayOutputStream buffer = new ByteArrayOutputStream();
    private final DataOutput output = new DataOutputStream(buffer);
    private final IBridge bridge;
    private final Cache objectIdCache;
    private final Cache threadIdCache;
    private final Cache typeCache;
}
