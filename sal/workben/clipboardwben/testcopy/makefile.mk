#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.5 $
#
#   last change: $Author: vg $ $Date: 2007-03-26 14:28:07 $
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

PRJ=..$/..$/..

PRJNAME=sal
TARGET=cbcpytest
LIBTARGET=NO

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

CFLAGS+= $(LFS_CFLAGS)
CXXFLAGS+= $(LFS_CFLAGS)

# --- Files --------------------------------------------------------

#
# test clipboard paste
#
    # --- Resources ----------------------------------------------------
    RCFILES=  cbcpytest.rc

    OBJFILES= $(OBJ)$/cbcpytest.obj $(OBJ)$/XTDataObject.obj

    APP1TARGET=	$(TARGET)
    APP1OBJS=	$(OBJFILES)
    APP1NOSAL=  TRUE
    APP1NOSVRES= $(RES)$/$(TARGET).res

    APP1STDLIBS+=$(OLE32LIB) $(USER32LIB) $(KERNEL32LIB) $(SALLIB)
    
    APP1LIBS=$(LB)$/iole9x.lib \
             $(LB)$/tools32.lib 

    APP1DEPN=   makefile.mk $(APP1NOSVRES)

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk


