#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.8 $
#
#   last change: $Author: vg $ $Date: 2007-02-06 13:36:18 $
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
TARGET=regmerge
TARGETTYPE=CUI
LIBTARGET=NO

ENABLE_EXCEPTIONS := TRUE

.IF "$(OS)" == "LINUX"
LINKFLAGSRUNPATH = -Wl,-rpath,\''$$ORIGIN/../lib:$$ORIGIN'\'
.ELIF "$(OS)" == "SOLARIS"
LINKFLAGSRUNPATH = -R\''$$ORIGIN/../lib:$$ORIGIN'\'
.ENDIF

# --- Settings -----------------------------------------------------
.INCLUDE :  settings.mk

.INCLUDE :  ..$/version.mk

# --- Files --------------------------------------------------------
CDEFS += -DDLL_VERSION=\"$(UPD)$(DLLPOSTFIX)\"

CXXFILES=	regview.cxx	  	\
            regmerge.cxx	\
            regcompare.cxx


APP1TARGET= $(TARGET)
APP1OBJS=   $(OBJ)$/regmerge.obj 

APP1STDLIBS=\
            $(SALLIB) \
            $(SALHELPERLIB) \
            $(REGLIB)

APP2TARGET= regview
APP2OBJS=   $(OBJ)$/regview.obj

APP2STDLIBS=\
            $(SALLIB) \
            $(SALHELPERLIB) \
            $(REGLIB)

APP3TARGET= regcompare
APP3OBJS=   $(OBJ)$/regcompare.obj

APP3STDLIBS=\
            $(SALLIB) \
            $(SALHELPERLIB) \
            $(REGLIB)

APP4TARGET= checksingleton
APP4OBJS=   $(OBJ)$/checksingleton.obj

APP4STDLIBS=\
            $(SALLIB) \
            $(SALHELPERLIB) \
            $(REGLIB)

.INCLUDE :  target.mk
