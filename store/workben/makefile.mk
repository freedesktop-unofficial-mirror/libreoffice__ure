#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2008 by Sun Microsystems, Inc.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# $RCSfile: makefile.mk,v $
#
# $Revision: 1.6 $
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

PRJ=..

PRJNAME=store
TARGET=workben

LIBTARGET=NO
TARGETTYPE=CUI

# --- Settings ---

.INCLUDE : settings.mk

.IF "$(STORELIB)" == ""
.IF "$(GUI)" == "UNX"
STORELIB=	-lstore
.ELSE  # unx
STORELIB=	$(LB)$/istore.lib
.ENDIF # unx
.ENDIF # storelib

.IF "$(GUI)" == "UNX"
STOREDBGLIB=	$(LB)$/libstoredbg.a
.ELSE  # unx
STOREDBGLIB=	$(LB)$/storedbg.lib
.ENDIF # unx

CFLAGS+= -I..$/source

# --- Files ---

CXXFILES=	\
    t_file.cxx	\
    t_base.cxx	\
    t_store.cxx

OBJFILES=	\
    $(OBJ)$/t_file.obj	\
    $(OBJ)$/t_base.obj	\
    $(OBJ)$/t_store.obj

APP1TARGET=		t_file
APP1OBJS=		$(OBJ)$/t_file.obj
APP1STDLIBS=	$(STOREDBGLIB)
APP1STDLIBS+=	$(SALLIB)
APP1DEPN=	\
    $(STOREDBGLIB)	\
    $(L)$/isal.lib

APP2TARGET=		t_base
APP2OBJS=		$(OBJ)$/t_base.obj
APP2STDLIBS=	$(STOREDBGLIB)
APP2STDLIBS+=	$(SALLIB)
APP2DEPN=	\
    $(STOREDBGLIB)	\
    $(L)$/isal.lib

APP3TARGET=		t_store
APP3OBJS=		$(OBJ)$/t_store.obj
APP3STDLIBS=	$(STORELIB)
APP3STDLIBS+=	$(SALLIB)
APP3DEPN=	\
    $(SLB)$/store.lib	\
    $(L)$/isal.lib

# --- Targets ---

.INCLUDE : target.mk

