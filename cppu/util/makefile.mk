#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.8 $
#
#   last change: $Author: hr $ $Date: 2006-04-19 13:50:07 $
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

PRJNAME=cppu
TARGET=cppu
USE_DEFFILE=TRUE
ENABLE_EXCEPTIONS=TRUE
NO_BSYMBOLIC=TRUE

UNIXVERSIONNAMES=UDK

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# --- Files --------------------------------------------------------

SHL1LIBS= \
    $(SLB)$/cppu_typelib.lib	\
    $(SLB)$/cppu_uno.lib		\
    $(SLB)$/cppu_threadpool.lib	\
    $(SLB)$/cppu_cppu.lib

.IF "$(GUI)" == "WNT"
SHL1TARGET=$(TARGET)
.ELSE
SHL1TARGET= uno_$(TARGET)
.ENDIF

SHL1STDLIBS = $(SALLIB) $(SALHELPERLIB)

SHL1DEPN=
SHL1IMPLIB=i$(TARGET)
.IF "$(OS)"!="FREEBSD"
SHL1DEF=$(MISC)$/$(SHL1TARGET).def
.ENDIF

SHL1VERSIONMAP=$(TARGET).map

DEF1NAME=$(SHL1TARGET)

# --- Targets ------------------------------------------------------

.INCLUDE :	target.mk

