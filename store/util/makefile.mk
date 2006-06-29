#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.17 $
#
#   last change: $Author: ihi $ $Date: 2006-06-29 11:27:36 $
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

PRJ=..

PRJNAME=store
TARGET=store
TARGETTYPE=CUI

USE_LDUMP2=TRUE
USE_DEFFILE=TRUE

NO_BSYMBOLIC=TRUE
NO_DEFAULT_STL=TRUE

UNIXVERSIONNAMES=UDK

# --- Settings ---

.INCLUDE : settings.mk

# --- Debug-Library ---

.IF "$(debug)" != ""

LIB1TARGET=	$(LB)$/$(TARGET)dbg.lib
LIB1ARCHIV=	$(LB)$/lib$(TARGET)dbg.a
LIB1FILES=	$(LB)$/store.lib

.ENDIF # debug

# --- Shared-Library ---

SHL1TARGET= 	$(TARGET)
SHL1IMPLIB= 	istore
.IF "$(OS)"=="MACOSX"
#SHL1VERSIONMAP=	$(TARGET).$(DLLPOSTFIX).map
.ELSE
SHL1VERSIONMAP=	$(TARGET).map
.ENDIF

SHL1STDLIBS=	$(SALLIB)

.IF "$(USE_STLP_DEBUG)" != ""
SHL1STDLIBS+=$(LIBSTLPORT)
.ENDIF

# On gcc3 __Unwind_SetIP is not in supc++ but in libgcc_s.so
.IF "$(COMID)"=="gcc3"
.IF "$(OS)"=="FREEBSD" || "$(OS)"=="NETBSD"
SHL1STDLIBS+= 	-lsupc++
.ELIF "$(OS)"=="MACOSX"
.IF "$(CCNUMVER)"<="000399999999"
SHL1STDLIBS+=	-lsupc++
.ENDIF # CCNUMVER
.ELIF "$(CCNUMVER)"<="000400000999"
SHL1STDLIBS+= 	-lsupc++ -lgcc_s
.ENDIF
.ENDIF


SHL1DEF=	$(MISC)$/$(SHL1TARGET).def
SHL1LIBS=	$(SLB)$/store.lib

# --- Def-File ---

DEF1NAME=	$(SHL1TARGET)
DEF1DES=Store

# --- Targets ---

.INCLUDE : target.mk



