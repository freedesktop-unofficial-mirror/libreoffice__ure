/*************************************************************************
 *
 *  $RCSfile: string.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: pl $ $Date: 2000-11-15 11:36:22 $
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

#ifndef _RTL_STRING_HXX_
#define _RTL_STRING_HXX_

#ifndef _RTL_STRING_H_
#include <rtl/string.h>
#endif
#ifndef _RTL_USTRING_H_
#include <rtl/ustring.h>
#endif
#ifndef _RTL_MEMORY_H_
#include <rtl/memory.h>
#endif
#ifndef _RTL_TEXTENC_H
#include <rtl/textenc.h>
#endif
#ifndef _RTL_MACROS_HXX_
#include <rtl/macros.hxx>
#endif

#ifdef __cplusplus

class ByteString;

#ifdef _USE_NAMESPACE
namespace rtl
{
#endif

/**
 * The <code>OAsciiString</code> class represents character strings. All
 * string literals in Java programs, such as <code>"abc"</code>, are
 * implemented as instances of this class.
 * <p>
 * Strings are constant; their values cannot be changed after they
 * are created. OAsciiString buffers support mutable strings.
 * Because OAsciiString objects are immutable they can be shared. For example:
 * <p><blockquote><pre>
 *	   OAsciiString str = "abc";
 * </pre></blockquote><p>
 * is equivalent to:
 * <p><blockquote><pre>
 *	   char data[] = {'a', 'b', 'c'};
 *	   OAsciiString str = new OAsciiString(data);
 * </pre></blockquote><p>
 * Here are some more examples of how strings can be used:
 * <p><blockquote><pre>
 *	   System.out.println("abc");
 *	   OAsciiString cde = "cde";
 *	   System.out.println("abc" + cde);
 *	   OAsciiString c = "abc".substring(2,3);
 *	   OAsciiString d = cde.substring(1, 2);
 * </pre></blockquote>
 * <p>
 * The class <code>OAsciiString</code> includes methods for examining
 * individual characters of the sequence, for comparing strings, for
 * searching strings, for extracting substrings, and for creating a
 * copy of a string with all characters translated to uppercase or to
 * lowercase.
 */
class OString
{
    class DO_NOT_ACQUIRE;
    OString( rtl_String * value, DO_NOT_ACQUIRE * )
        : pData( value )
    {
    }
public:
    /**
     * A pointer to the data structur which contains the data.
     */
    rtl_String * pData;

    /**
     * Allocates a new <code>OString</code> containing no characters.
     */
    OString()
        : pData(NULL)
    {
        RTL_STRING_NEW(&pData);
    }

    /**
     * Notice and acquire the string data of value.
     *
     * @param	value	a <code>OString</code>.
     */
    OString( const OString & value )
        : pData( value.pData )
    {
        rtl_string_acquire( pData );
    }

    /**
     * Notice and acquire the string data of value.
     *
     * @param	value	a <code>rtl_String</code>.
     */
    OString( rtl_String * value )
        : pData(value)
    {
        rtl_string_acquire( pData );
    }

    /**
     * Allocates a new <code>OString</code> so that it represents the
     * sequence of characters currently contained in the character array
     * argument.
     *
     * @param  value   the initial value of the string.
     */
    OString( const sal_Char * value)
        : pData(NULL)
    {
        rtl_string_newFromStr( &pData, value );
    }

    /**
     * Allocates a new <code>OString</code> that contains characters from
     * a subarray of the character array argument. The <code>offset</code>
     * argument is the index of the first character of the subarray and
     * the <code>count</code> argument specifies the length of the
     * subarray.
     *
     * @param	   value	array that is the source of characters.
     * @param	   offset	the initial offset.
     * @param	   count	the length.
     * @exception  StringIndexOutOfBoundsException	if the <code>offset</code>
     *				 and <code>count</code> arguments index characters outside
     *				 the bounds of the <code>value</code> array.
     */
    OString( const sal_Char * value, sal_Int32 length )
        : pData(NULL)
    {
        rtl_string_newFromStr_WithLength( &pData, value, length );
    }

    /**
     * Allocates a new <code>OString</code> that contains the first
     * <code>count</code> characters of the Unicode character array argument.
     * The Unicode string is converted to a 8 bit string using the
     * <code>encoding</code> argument.
     *
     * @param	   value	Unicode character array
     * @param	   count	the length.
     * @param	   encoding TextEncoding used to convert the 8 bit string
     * @exception  StringIndexOutOfBoundsException	<code>count</code> argument
     *				is bigger than the length of the 8 bit character string.
     */
    OString( const sal_Unicode * value, sal_Int32 length, rtl_TextEncoding encoding )
        : pData(NULL)
    {
        RTL_STRING_NEW(&pData);
        if( value )
        {
            // for exception
            //sal_Int32 len = rtl_ustr_getLength(value);
            //if( len < length )
            //	throw StringIndexOutOfBoundsException;

            if( length )
                rtl_uString2String( &pData, value, length, encoding, OUSTRING_TO_OSTRING_CVTFLAGS );
        }
    }

    OString( const ByteString & value );

    /**
     * Release the string own string data and notice and acquire the string data of value.
     */
    OString& operator = ( const OString& value )
    {
        rtl_string_assign( &pData, value.pData );
        return *this;
    }

    /**
     * Release the string own string data and replace the string data with new string data
     * containing the old string data concatenate with the sting data of value.
     */
    OString operator+=( const OString& rStr )
    {
        rtl_string_newConcat( &pData, pData, rStr.pData );
        return *this;
    }

    /**
     * Release the string data.
     */
    ~OString()
    {
        rtl_string_release( pData );
    }

    /**
     * Allocates a new string that contains the sequence of characters
     * currently contained in the string buffer argument.
     *
     * @param	buffer	 a <code>StringBuffer</code>.
     */
    //public OString (StringBuffer buffer) {

    /**
     * Returns the length of this string.
     * The length is equal to the number of 16-bit
     * Unicode characters in the string.
     *
     * @return	the length of the sequence of characters represented by this
     *			object.
     */
    sal_Int32 getLength() const { return pData->length; }

    /**
     * Returns the character at the specified index. An index ranges
     * from <code>0</code> to <code>length() - 1</code>.
     *
     * @param	   index   the index of the character.
     * @return	   the character at the specified index of this string.
     *			   The first character is at index <code>0</code>.
     * @exception  StringIndexOutOfBoundsException	if the index is out of
     *				 range.
     */
    //sal_Char	operator [] ( sal_Int32 nIndex ) const
    //	{ return pData->buffer[nIndex]; }

    /**
     * Return a null terminated character array.
     */
    operator		const sal_Char *() const { return pData->buffer; }

    /**
     * Return a null terminated unicode character array.
     */
    const sal_Char* getStr() const { return pData->buffer; }

    /**
     * Compares this string to the specified object.
     * The result is <code>true</code> if and only if the argument is not
     * <code>null</code> and is a <code>OString</code> object that represents
     * the same sequence of characters as this object.
     *
     * @param	anObject   the object to compare this <code>OString</code>
     *					   against.
     * @return	<code>true</code> if the <code>OString </code>are equal;
     *			<code>false</code> otherwise.
     * @see 	java.lang.OString#compareTo(java.lang.OString)
     * @see 	java.lang.OString#equalsIgnoreCase(java.lang.OString)
     */
    sal_Bool equals( const OString & rObj ) const
    {
        return getLength() == rObj.getLength() && compareTo( rObj ) == 0;
    }

    /**
     * Compares this OString to another object.
     * The result is <code>true</code> if and only if the argument is not
     * <code>null</code> and is a <code>OString</code> object that represents
     * the same sequence of characters as this object, where case is ignored.
     * <p>
     * Two characters are considered the same, ignoring case, if at
     * least one of the following is true:
     * <ul>
     * <li>The two characters are the same (as compared by the <code>==</code>
     *	   operator).
     * <li>Applying the method <code>Character.toUppercase</code> to each
     *	   character produces the same result.
     * <li>Applying the method <code>Character.toLowercase</code> to each
     *	   character produces the same result.
     * </ul>
     * <p>
     * Two sequences of characters are the same, ignoring case, if the
     * sequences have the same length and corresponding characters are
     * the same, ignoring case.
     *
     * @param	anotherString	the <code>OString</code> to compare this
     *							<code>OString</code> against.
     * @return	<code>true</code> if the <code>OString</code>s are equal,
     *			ignoring case; <code>false</code> otherwise.
     * @see 	java.lang.Character#toLowerCase(char)
     * @see 	java.lang.Character#toUpperCase(char)
     */
    sal_Bool equalsIgnoreCase( const OString & rObj ) const
    {
        return rtl_str_equalsIgnoreCase_WithLength( pData->buffer, pData->length,
                                rObj.pData->buffer, rObj.pData->length );
    }

    /**
     * Compares two strings lexicographically.
     * The comparison is based on the Unicode value of each character in
     * the strings.
     *
     * @param	anotherString	the <code>OString</code> to be compared.
     * @return	the value <code>0</code> if the argument string is equal to
     *			this string; a value less than <code>0</code> if this string
     *			is lexicographically less than the string argument; and a
     *			value greater than <code>0</code> if this string is
     *			lexicographically greater than the string argument.
     */
    sal_Int32 compareTo( const OString & rObj ) const
    {
        return rtl_str_compare_WithLength( pData->buffer, pData->length,
                                rObj.pData->buffer, rObj.pData->length );
    }

    /**
     * Compares two strings lexicographically using only the number of
     * characters given in the length parameter.
     * The comparison is based on the Unicode value of each character in
     * the strings.
     *
     * @param	anotherString	the <code>OString</code> to be compared.
     * @param	length	  the number of characters that are compared.
     * @return	'This string' and 'argument string' here means the substring
     *			defined by the length parameter.
     *			the value <code>0</code> if the argument string is equal to
     *			this string; a value less than <code>0</code> if this string
     *			is lexicographically less than the string argument; and a
     *			value greater than <code>0</code> if this string is
     *			lexicographically greater than the string argument.
     */
    sal_Int32 compareTo( const OString & rObj, sal_Int32 length ) const
    {
        return rtl_str_compare_WithLength( pData->buffer, length,
                                rObj.pData->buffer, rObj.pData->length );
    }

    friend sal_Bool 	operator == ( const OString& rStr1, const OString& rStr2 )
                        { return rStr1.getLength() == rStr2.getLength() && rStr1.compareTo( rStr2 ) == 0; }
    friend sal_Bool 	operator == ( const OString& rStr1, const sal_Char * pStr2 )
                        { return rStr1.compareTo( pStr2 ) == 0; }
    friend sal_Bool 	operator == ( const sal_Char * pStr1,	const OString& rStr2 )
                        { return OString( pStr1 ).compareTo( rStr2 ) == 0; }

    friend sal_Bool 	operator != ( const OString& rStr1, 	const OString& rStr2 )
                        { return !(operator == ( rStr1, rStr2 )); }
    friend sal_Bool 	operator != ( const OString& rStr1, const sal_Char * pStr2 )
                        { return !(operator == ( rStr1, pStr2 )); }
    friend sal_Bool 	operator != ( const sal_Char * pStr1,	const OString& rStr2 )
                        { return !(operator == ( pStr1, rStr2 )); }

    friend sal_Bool 	operator <	( const OString& rStr1,    const OString& rStr2 )
                        { return rStr1.compareTo( rStr2 ) < 0; }
    friend sal_Bool 	operator >	( const OString& rStr1,    const OString& rStr2 )
                        { return rStr1.compareTo( rStr2 ) > 0; }
    friend sal_Bool 	operator <= ( const OString& rStr1,    const OString& rStr2 )
                        { return rStr1.compareTo( rStr2 ) <= 0; }
    friend sal_Bool 	operator >= ( const OString& rStr1,    const OString& rStr2 )
                        { return rStr1.compareTo( rStr2 ) >= 0; }

    /**
     * Returns a hashcode for this string.
     *
     * @return	a hash code value for this object.
     */
    sal_Int32 hashCode() const
    {
        return rtl_str_hashCode_WithLength( pData->buffer, pData->length );
    }

    /**
     * Returns the index within this string of the first occurrence of the
     * specified character, starting the search at the specified index.
     *
     * @param	ch			a character.
     * @param	fromIndex	the index to start the search from.
     * @return	the index of the first occurrence of the character in the
     *			character sequence represented by this object that is greater
     *			than or equal to <code>fromIndex</code>, or <code>-1</code>
     *			if the character does not occur.
     */
    sal_Int32 indexOf( sal_Char ch, sal_Int32 fromIndex = 0 ) const
    {
        sal_Int32 ret = rtl_str_indexOfChar_WithLength( pData->buffer + fromIndex, pData->length - fromIndex, ch );
        return (ret < 0 ? ret : ret + fromIndex);
    }

    /**
     * Returns the index within this string of the first occurence of
     * the specified substring.
     * The returned index indicates the start of the substring, and it
     * must be equal to or less than <code>fromIndex</code>.
     *
     * @param	str 		the substring to search for.
     * @param	fromIndex	the index to start the search from.
     * @return	If the string argument occurs one or more times as a substring
     *			within this object at a starting index no greater than
     *			<code>fromIndex</code>, then the index of the first character of
     *			the last such substring is returned. If it does not occur as a
     *			substring starting at <code>fromIndex</code> or earlier,
     *			<code>-1</code> is returned.
     */
    sal_Int32 indexOf( const OString & str, int fromIndex = 0) const
    {
        sal_Int32 ret = rtl_str_indexOfStr_WithLength( pData->buffer + fromIndex, pData->length - fromIndex,
                                                       str.pData->buffer, str.pData->length );
        return (ret < 0 ? ret : ret + fromIndex);
    }

    /**
     * Returns the index within this string of the last occurrence of the
     * specified character, searching backward starting at the end.
     *
     * @param	ch			a character.
     * @param	fromIndex	the index to start the search from.
     * @return	the index of the last occurrence of the character in the
     *			character sequence represented by this object that is less
     *			than or equal to <code>length -1</code>, or <code>-1</code>
     *			if the character does not occur before that point.
     */
    sal_Int32 lastIndexOf( sal_Char ch, int fromIndex ) const
    {
        return rtl_str_lastIndexOfChar_WithLength( pData->buffer, fromIndex, ch );
    }

    /**
     * Returns the index within this string of the last occurrence of the
     * specified character, searching backward starting at the end.
     *
     * @param	ch			a character.
     * @param	fromIndex	the index to start the search from.
     * @return	the index of the last occurrence of the character in the
     *			character sequence represented by this object that is less
     *			than or equal to <code>length -1</code>, or <code>-1</code>
     *			if the character does not occur before that point.
     */
    sal_Int32 lastIndexOf( sal_Char ch ) const
    {
        return rtl_str_lastIndexOfChar_WithLength( pData->buffer, pData->length, ch );
    }

    /**
     * Returns the index within this string of the last occurrence of
     * the specified substring.
     * The returned index indicates the start of the substring, and it
     * must be equal to or less than <code>fromIndex</code>.
     *
     * @param	str 		the substring to search for.
     * @param	fromIndex	the index to start the search from.
     * @return	If the string argument occurs one or more times as a substring
     *			within this object at a starting index no greater than
     *			<code>fromIndex</code>, then the index of the first character of
     *			the last such substring is returned. If it does not occur as a
     *			substring starting at <code>fromIndex</code> or earlier,
     *			<code>-1</code> is returned.
     */
    sal_Int32 lastIndexOf( const OString & str, int fromIndex ) const
    {
        return rtl_str_lastIndexOfStr_WithLength( pData->buffer, fromIndex,
                                                str.pData->buffer, str.pData->length );
    }

    /**
     * Returns the index within this string of the last occurrence of
     * the specified substring.
     * The returned index indicates the start of the substring, and it
     * must be equal to or less than <code>fromIndex</code>.
     *
     * @param	str 		the substring to search for.
     * @param	fromIndex	the index to start the search from.
     * @return	If the string argument occurs one or more times as a substring
     *			within this object at a starting index no greater than
     *			<code>fromIndex</code>, then the index of the first character of
     *			the last such substring is returned. If it does not occur as a
     *			substring starting at <code>fromIndex</code> or earlier,
     *			<code>-1</code> is returned.
     */
    sal_Int32 lastIndexOf( const OString & str ) const
    {
        return rtl_str_lastIndexOfStr_WithLength( pData->buffer, pData->length,
                                                str.pData->buffer, str.pData->length );
    }

    /**
     * Returns a new string that is a substring of this string. The
     * substring begins at the specified index and extends to the end of
     * this string.
     *
     * @param	   beginIndex	the beginning index, inclusive.
     * @return	   the specified substring.
     * @exception  StringIndexOutOfBoundsException	if the
     *			   <code>beginIndex</code> is out of range.
     */
    OString copy( sal_Int32 beginIndex) const
    {
        return copy( beginIndex, getLength() - beginIndex );
    }


    /**
     * Returns a new string that is a substring of this string. The
     * substring begins at the specified <code>beginIndex</code> and
     * extends to the character at index <code>endIndex - 1</code>.
     *
     * @param	   beginIndex	the beginning index, inclusive.
     * @param	   count		the number of characters.
     * @return	   the specified substring.
     * @exception  StringIndexOutOfBoundsException	if the
     *			   <code>beginIndex</code> or the <code>endIndex</code> is
     *			   out of range.
     */
    OString copy( sal_Int32 beginIndex, sal_Int32 count ) const
    {
        if( beginIndex == 0 && count == getLength() )
            return *this;
        else
        {
            OString newStr;
            rtl_string_newFromStr_WithLength( &newStr.pData, pData->buffer + beginIndex, count );
            return newStr;
        }
    }

    /**
     * Concatenates the specified string to the end of this string.
     * <p>
     * If the length of the argument string is <code>0</code>, then this
     * object is returned.
     *
     * @param	str   the <code>OString</code> that is concatenated to the end
     *				  of this <code>OString</code>.
     * @return	a string that represents the concatenation of this object's
     *			characters followed by the string argument's characters.
     */
    OString concat( const OString & str ) const
    {
        OString newStr;
        rtl_string_newConcat( &newStr.pData, pData, str.pData );
        return newStr;
    }

    friend OString	operator+( const OString& rStr1, const OString& rStr2  )
    {
        return rStr1.concat( rStr2 );
    }

    /**
     * Returns a new string resulting from replacing all occurrences of
     * <code>oldChar</code> in this string with <code>newChar</code>.
     * <p>
     * If the character <code>oldChar</code> does not occur in the
     * character sequence represented by this object, then this string is
     * returned.
     *
     * @param	oldChar   the old character.
     * @param	newChar   the new character.
     * @return	a string derived from this string by replacing every
     *			occurrence of <code>oldChar</code> with <code>newChar</code>.
     */
    OString replace( sal_Char oldChar, sal_Char newChar ) const
    {
        rtl_String * pNew = 0;
        rtl_string_newReplace( &pNew, pData, oldChar, newChar );
        return OString( pNew, (DO_NOT_ACQUIRE *)0 );
    }

    /**
     * Returns a new string resulting from replacing n = count characters
     * from position index in this string with <code>newStr</code>.
     * <p>
     *
     * @param	index	the index for beginning.
     * @param	count	the count of charcters that will replaced
     * @param	newStr	the new substring.
     * @return	the new string. The reference count is 1.
     */
    OString replaceAt(sal_Int32 index, sal_Int32 count, const OString& newStr ) const
    {
        rtl_String * pNew = 0;
        rtl_string_newReplaceStrAt( &pNew, pData, index, count, newStr.pData );
        return OString( pNew, (DO_NOT_ACQUIRE *)0 );
    }

    /**
     * Converts this <code>OString</code> to lowercase.
     * <p>
     * If no character in the string has a different lowercase version,
     * based on calling the <code>toLowerCase</code> method defined by
     * <code>Character</code>, then the original string is returned.
     * <p>
     * Otherwise, a new string is allocated, whose length is identical
     * to this string, and such that each character that has a different
     * lowercase version is mapped to this lowercase equivalent.
     *
     * @return	the string, converted to lowercase.
     * @see 	java.lang.Character#toLowerCase(char)
     * @see 	java.lang.OString#toUpperCase()
     */
    OString toLowerCase() const
    {
        rtl_String * pNew = 0;
        rtl_string_newToLowerCase( &pNew, pData );
        return OString( pNew, (DO_NOT_ACQUIRE *)0 );
    }

    /**
     * Converts this string to uppercase.
     * <p>
     * If no character in this string has a different uppercase version,
     * based on calling the <code>toUpperCase</code> method defined by
     * <code>Character</code>, then the original string is returned.
     * <p>
     * Otherwise, a new string is allocated, whose length is identical
     * to this string, and such that each character that has a different
     * uppercase version is mapped to this uppercase equivalent.
     *
     * @return	the string, converted to uppercase.
     * @see 	java.lang.Character#toUpperCase(char)
     * @see 	java.lang.OString#toLowerCase()
     */
    OString toUpperCase() const
    {
        rtl_String * pNew = 0;
        rtl_string_newToUpperCase( &pNew, pData );
        return OString( pNew, (DO_NOT_ACQUIRE *)0 );
    }

    /**
     * Removes white space from both ends of this string.
     * <p>
     * All characters that have codes less than or equal to
     * <code>'&#92;u0020'</code> (the space character) are considered to be
     * white space.
     *
     * @return	this string, with white space removed from the front and end.
     */
    OString trim() const
    {
        rtl_String * pNew = 0;
        rtl_string_newTrim( &pNew, pData );
        return OString( pNew, (DO_NOT_ACQUIRE *)0 );
    }

    /**
     * Returns the string representation of the <code>sal_Bool</code> argument.
     *
     * @param	b	a <code>sal_Bool</code>.
     * @return	if the argument is <code>true</code>, a string equal to
     *			<code>"true"</code> is returned; otherwise, a string equal to
     *			<code>"false"</code> is returned.
     */
    static OString valueOf(sal_Bool b)
    {
        sal_Char sz[RTL_STR_MAX_VALUEOFBOOLEAN];
        rtl_String * pNew = 0;
        rtl_string_newFromStr_WithLength( &pNew, sz, rtl_str_valueOfBoolean( sz, b ) );
        return OString( pNew, (DO_NOT_ACQUIRE *)0 );
    }

    /**
     * Returns the string representation of the <code>char</code> * argument.
     *
     * @param	c	a <code>char</code>.
     * @return	a newly allocated string of length <code>1</code> containing
     *			as its single character the argument <code>c</code>.
     */
    static OString valueOf(sal_Char c)
    {
        return OString( &c, 1 );
    }

    /**
     * Returns the string representation of the <code>int</code> argument.
     * <p>
     * The representation is exactly the one returned by the
     * <code>Integer.toString</code> method of one argument.
     *
     * @param	i	an <code>int</code>.
     * @return	a newly allocated string containing a string representation of
     *			the <code>int</code> argument.
     * @see 	java.lang.Integer#toString(int, int)
     */
    static OString valueOf(sal_Int32 i, sal_Int16 radix = 10 )
    {
        sal_Char sz[RTL_STR_MAX_VALUEOFINT32];
        rtl_String * pNew = 0;
        rtl_string_newFromStr_WithLength( &pNew, sz, rtl_str_valueOfInt32( sz, i, radix ) );
        return OString( pNew, (DO_NOT_ACQUIRE *)0 );
    }

    /**
     * Returns the string representation of the <code>long</code> argument.
     * <p>
     * The representation is exactly the one returned by the
     * <code>Long.toString</code> method of one argument.
     *
     * @param	l	a <code>long</code>.
     * @return	a newly allocated string containing a string representation of
     *			the <code>long</code> argument.
     * @see 	java.lang.Long#toString(long)
     */
    static OString valueOf(sal_Int64 l, sal_Int16 radix = 10 )
    {
        sal_Char sz[RTL_STR_MAX_VALUEOFINT64];
        rtl_String * pNew = 0;
        rtl_string_newFromStr_WithLength( &pNew, sz, rtl_str_valueOfInt64( sz, l, radix ) );
        return OString( pNew, (DO_NOT_ACQUIRE *)0 );
    }

    /**
     * Returns the string representation of the <code>float</code> argument.
     * <p>
     * The representation is exactly the one returned by the
     * <code>Float.toString</code> method of one argument.
     *
     * @param	f	a <code>float</code>.
     * @return	a newly allocated string containing a string representation of
     *			the <code>float</code> argument.
     * @see 	java.lang.Float#toString(float)
     */
    static OString valueOf(float f)
    {
        sal_Char sz[RTL_STR_MAX_VALUEOFFLOAT];
        rtl_String * pNew = 0;
        rtl_string_newFromStr_WithLength( &pNew, sz, rtl_str_valueOfFloat( sz, f ) );
        return OString( pNew, (DO_NOT_ACQUIRE *)0 );
    }

    /**
     * Returns the string representation of the <code>double</code> argument.
     * <p>
     * The representation is exactly the one returned by the
     * <code>Double.toString</code> method of one argument.
     *
     * @param	d	a <code>double</code>.
     * @return	a newly allocated string containing a string representation of
     *			the <code>double</code> argument.
     * @see 	java.lang.Double#toString(double)
     */
    static OString valueOf(double d)
    {
        sal_Char sz[RTL_STR_MAX_VALUEOFDOUBLE];
        rtl_String * pNew = 0;
        rtl_string_newFromStr_WithLength( &pNew, sz, rtl_str_valueOfDouble( sz, d ) );
        return OString( pNew, (DO_NOT_ACQUIRE *)0 );
    }

    // UString compatibility deprecated
    sal_Int32			len() const { return getLength(); }

    sal_Int32			search( sal_Char ch ) const { return indexOf( ch ); }

    sal_Int32			getTokenCount( sal_Char cTok = ';' ) const
    {
            return rtl_string_getTokenCount(pData, cTok );
    }

    OString 			getToken( sal_Int32 nToken, sal_Char cTok = ';' ) const
    {
            OString newStr;
            rtl_string_getToken( &newStr.pData, pData, nToken, cTok );
            return newStr;
    }

    sal_Int32			search( const OString& rStr, sal_Int32 nIndex = 0) const { return indexOf( rStr, nIndex ); }
    sal_Int32			search( const sal_Char* pchar, sal_Int32 nIndex = 0) const { return indexOf( pchar, nIndex ); }
};

struct OStringHash
{
     size_t operator()(const rtl::OString& rString) const
         { return (size_t)rString.hashCode(); }
};

#ifdef _USE_NAMESPACE
}
#endif

#endif /* __cplusplus */
#endif /* _RTL_STRING_HXX_ */



