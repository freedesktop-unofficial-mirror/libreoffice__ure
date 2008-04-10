/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: sistr.hxx,v $
 * $Revision: 1.5 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#ifndef XML2CMP_SISTR_HXX
#define XML2CMP_SISTR_HXX


class  Simstr
{
// INTERFACE
   public:
      // Constructors, destructor, '=' and typecasts
                     Simstr(
                           const char *   str = 0);
                     Simstr(                       // Creates Simstr of 'anzahl' times 'txt'.
                           const char *   txt,
                           int            anzahl);
                     Simstr(
                           char           c,       // Creates Simstr of 'anzahl' times 'c'.
                           int            anzahl);
                     Simstr(                       // Creates Simstr out of a copy of the described bytes within 'anyBytes'.
                                                   // Adds a '\0' at the end.
                           const char *   anybytes,
                           int			  firstBytesPos,
                           int            nrOfBytes);
      virtual        ~Simstr();
                     Simstr(
                           const Simstr & S);
      Simstr &       operator=(
                           const Simstr & S);
                     operator const char*() const;

      // diverse utility functions
      const char *   str() const				{ return sz; }
      char *         s();   // ATTENTION !!!       // Only to be used, when a function needs a 'char*' but
                                                   //   nevertheless THAT WILL BE NOT CHANGED!
                                                   //   Typecasts to 'const char*' are performed automatically.
      int            l() const;                    // Length of string without '\0' at end.
      char &         ch(   int            n);      // Reference to sz[n]. Allows change of this char.
                                                   //   !!! No safety, if n is out of the allowed range!
      Simstr         operator+(
                           const Simstr & S) const;
      Simstr &       operator+=(
                           const Simstr & S);

      // comparison operators
      bool           operator==(
                           const Simstr & S) const;
      bool           operator!=(
                           const Simstr & S) const;
      bool           operator<(
                           const Simstr & S) const;
      bool           operator>(
                           const Simstr & S) const;
      bool           operator<=(
                           const Simstr & S) const;
      bool           operator>=(
                           const Simstr & S) const;


      // 'List of characters' - functions
         // get - functions
      char           get(  int            n) const;
      char           get_front() const;
      char           get_back() const;
      Simstr         get(  int            startPos,
                           int            anzahl) const;
      Simstr         get_front(
                           int            anzahl) const;
      Simstr         get_back(
                           int            anzahl) const;
         // insert - functions
      void           insert(  int            pos,
                           char           c);
      void           push_front(
                           char           c);
      void           push_back(
                           char           c);
      void           insert(  int            pos,
                           const Simstr & S);
      void           push_front(
                           const Simstr & S);
      void           push_back(
                           const Simstr & S);
         // remove - functions
      void           remove(
                             int            pos,
                           int            anzahl = 1);
      void			 remove_trailing_blanks();
      void           pop_front(
                           int            anzahl = 1);
      void           pop_back(
                           int            anzahl = 1);
      void           rem_back_from(
                           int            removeStartPos);
      void           remove_all(
                           char           c);
      void           remove_all(                             // Starts search left.
                           const Simstr & S);
      void           strip(char           c);                // Removes all characters == c from front and back.
                                                             //   c == ' ' removes also TABs !!!
      void           empty();                                // Changes object to the value "".

         // search functions
      int            pos_first(
                           char           c) const;
      int            pos_first_after(                            // Sucht ab erstem char nach startSearchPos
                           char           c,
                           int            startSearchPos) const;
      int            pos_last(
                           char           c) const;
      int            pos_first(
                           const Simstr & S) const;
      int            pos_last(
                           const Simstr & S) const;
      int            count(
                             char           c) const;
      bool           is_empty() const;                         // Only true if object == "".
      bool           is_no_text() const;                       // String may contain spaces or tabs.

         // substitution functions
      void           replace(
                           int            pos,
                           char           c);
      void           replace(
                           int            startPos,
                           int            anzahl,
                           const Simstr & S);
      void           replace_all(
                           char           oldCh,
                           char           newCh);
      void           replace_all(
                           const Simstr & oldS,
                           const Simstr & newS);
      void           to_lower();

         // token functions
            // get...-functions return the token, separated by char 'c' and leave the object unchanged.
            // take...-functions return the same, but remove the token and the corresponding separator from the object.
      Simstr         get_first_token(
                           char           c) const;
      Simstr         get_last_token(
                           char           c) const;
      Simstr         take_first_token(
                           char           c);
      Simstr         take_last_token(
                           char           c);

   private:
      char *         sz;
      int            len;
};

// Simstr - char* / char - concatenations
Simstr operator+(const char * str, const Simstr & S);
Simstr operator+(const Simstr & S, const char * str);
Simstr operator+(char c, const Simstr & S);
Simstr operator+(const Simstr & S, char c);

// Simstr - char* - comparison operators
bool  operator==(const Simstr & S, const char * str);
bool  operator!=(const Simstr & S, const char * str);
bool  operator<(const Simstr & S, const char * str);
bool  operator>(const Simstr & S, const char * str);
bool  operator<=(const Simstr & S, const char * str);
bool  operator>=(const Simstr & S, const char * str);
bool  operator==(const char * str, const Simstr & S);
bool  operator!=(const char * str, const Simstr & S);
bool  operator<(const char * str, const Simstr & S);
bool  operator>(const char * str, const Simstr & S);
bool  operator<=(const char * str, const Simstr & S);
bool  operator>=(const char * str, const Simstr & S);


inline char *
Simstr::s()                            { return sz; }
inline int 
Simstr::l() const                      { return len; }
inline
Simstr::operator const char*() const   { return sz; }
inline bool
Simstr::is_empty() const               { return len == 0; }


#endif 

