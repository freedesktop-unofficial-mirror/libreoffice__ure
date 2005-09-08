#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.3 $
#
#   last change: $Author: rt $ $Date: 2005-09-08 16:49:34 $
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

PRJNAME=sal
TARGET=typesconfig
TARGETTYPE=CUI

LIBSALCPPRT=$(0)

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# --- Files --------------------------------------------------------

APP1TARGET=	$(TARGET)
APP1OBJS=		$(OBJ)$/typesconfig.obj
APP1STDLIBS=
APP1DEF=

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk

.IF "$(L10N-framework)"==""
ALLTAR : $(INCCOM)$/sal$/typesizes.h
.ENDIF			# "$(L10N-framework)"==""

$(INCCOM)$/sal$/typesizes.h : $(BIN)$/$(TARGET)
    +-$(MKDIR) $(INCCOM)$/sal
    $(BIN)$/$(TARGET) $@

