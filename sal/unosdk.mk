#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: unosdk.mk,v $
#
#   $Revision: 1.2 $
#
#   last change: $Author: rt $ $Date: 2005-09-08 14:24:25 $
#
#   The Contents of this file are made available subject to
#   the terms of GNU Lesser General Public License Version 2.1.
#
#
#     GNU Lesser General Public License Version 2.1
#     =============================================
#     Copyright 2005 by Sun Microsystems, Inc.
#     901 San Antonio Road, Palo Alto, CA 94303, USA
#
#     This library is free software; you can redistribute it and/or
#     modify it under the terms of the GNU Lesser General Public
#     License version 2.1, as published by the Free Software Foundation.
#
#     This library is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#     Lesser General Public License for more details.
#
#     You should have received a copy of the GNU Lesser General Public
#     License along with this library; if not, write to the Free Software
#     Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#     MA  02111-1307  USA
#
#*************************************************************************

PPATH=inc

FILES= \
    $(PPATH)\rtl\wstring 	\
    $(PPATH)\rtl\wstring.h 	\
    $(PPATH)\rtl\wstring.hxx \
    $(PPATH)\rtl\string.h 	\
    $(PPATH)\rtl\string.hxx 	\
    $(PPATH)\rtl\wstrbuf.hxx \
    $(PPATH)\rtl\strbuf.hxx 	\
    $(PPATH)\rtl\locale.h 	\
    $(PPATH)\rtl\locale.hxx \
    $(PPATH)\rtl\char.h 		\
    $(PPATH)\rtl\char.hxx 	\
    $(PPATH)\rtl\macros.hxx \
    $(PPATH)\rtl\object.h 	\
    $(PPATH)\rtl\rb.h 

unosdk: $(FILES)
     docpp -H -m -f  -u -d ..\..\doc\sal\rtl $(FILES)
