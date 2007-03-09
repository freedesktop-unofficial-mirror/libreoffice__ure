#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.5 $
#
#   last change: $Author: obo $ $Date: 2007-03-09 08:55:52 $
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

PRJNAME=registry
TARGET=regtest

USE_LDUMP2=TRUE
#LDUMP2=LDUMP3


# --- Settings -----------------------------------------------------
.INCLUDE :  settings.mk


# ------------------------------------------------------------------
CDEFS += -DDLL_VERSION=\"$(UPD)$(DLLPOSTFIX)\"

CXXFILES= \
               testregcpp.cxx	\
               testmerge.cxx		


LIB1TARGET= $(SLB)$/$(TARGET).lib

LIB1OBJFILES= \
                $(SLO)$/testregcpp.obj	\
                $(SLO)$/testmerge.obj


SHL1TARGET= rgt$(UPD)$(DLLPOSTFIX)
SHL1IMPLIB= rgt
SHL1STDLIBS=	\
                $(SALLIB) \
                $(SALHELPERLIB)	\
                $(STDLIBCPP)

SHL1LIBS=	$(LIB1TARGET)
SHL1DEPN=	$(LIB1TARGET)
SHL1DEF=	$(MISC)$/$(SHL1TARGET).def
DEF1NAME=	$(SHL1TARGET)
             
DEF1DEPN	=$(MISC)$/rgt$(UPD)$(DLLPOSTFIX).flt $(SLOFILES)
DEFLIB1NAME =$(TARGET)
DEF1DES 	=Registry Runtime - TestDll

# --- Targets ------------------------------------------------------

#all: \
#	ALLTAR	

.INCLUDE :  target.mk

# --- SO2-Filter-Datei ---


$(MISC)$/rgt$(UPD)$(DLLPOSTFIX).flt:
    @echo ------------------------------
    @echo Making: $@
    @echo WEP>$@
    @echo LIBMAIN>>$@
    @echo LibMain>>$@


