/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: filebuff.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 12:02:43 $
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

#ifndef X2C_FILEBUFF_HXX
#define X2C_FILEBUFF_HXX


class Buffer
{
  public:
                        Buffer() 			   	: s(0) {}
                        ~Buffer()               { if (s) delete [] s; }
                        operator const char *() const
                                                { return s; }
    char *				Data() 					{ return s; }
    void               	SetSize(
                            long 			    i_size )
                                                { if (s) delete [] s; s = new char [i_size]; }
  private:
    char * s;
};


bool				LoadXmlFile(
                        Buffer & 			o_rBuffer,
                        const char *		i_sXmlFilePath );



#endif

