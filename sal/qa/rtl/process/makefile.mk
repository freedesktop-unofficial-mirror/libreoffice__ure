#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2000, 2010 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
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

PRJ=..$/..$/..
INCPRE+= $(PRJ)$/qa$/inc

PRJNAME=sal
TARGET=rtl_Process

ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

CFLAGS+= $(LFS_CFLAGS)
CXXFLAGS+= $(LFS_CFLAGS)

.IF "$(SYSTEM_CPPUNIT)"=="YES"
CFLAGS+= $(CPPUNIT_CFLAGS)
CXXFLAGS+= $(CPPUNIT_CFLAGS)
.ENDIF

# BEGIN ----------------------------------------------------------------
# auto generated Target:joblist by codegen.pl
SHL1OBJS=  \
    $(SLO)$/rtl_Process.obj

SHL1TARGET= rtl_Process
SHL1STDLIBS= $(SALLIB) $(CPPUNITLIB) $(TESTSHL2LIB)

SHL1IMPLIB= i$(SHL1TARGET)
# SHL1DEF=    $(MISC)$/$(SHL1TARGET).def

DEF1NAME    =$(SHL1TARGET)
# DEF1EXPORTFILE= export.exp
SHL1VERSIONMAP= $(PRJ)$/qa$/export.map
# END ------------------------------------------------------------------

OBJ3FILES=$(OBJ)$/child_process.obj
APP3TARGET=child_process
APP3OBJS=$(OBJ3FILES)

#.IF "$(GUI)" == "UNX"
#APP3STDLIBS=$(LB)$/libsal.so
#.ENDIF
#.IF "$(GUI)" == "WNT"
#APP3STDLIBS=$(KERNEL32LIB) $(LB)$/isal.lib
#.ENDIF
APP3STDLIBS=$(SALLIB)

OBJ4FILES=$(OBJ)$/child_process_id.obj
APP4TARGET=child_process_id
APP4OBJS=$(OBJ4FILES)

# .IF "$(GUI)" == "UNX"
# APP4STDLIBS=$(LB)$/libsal.so
# .ENDIF
# .IF "$(GUI)" == "WNT"
# APP4STDLIBS=$(KERNEL32LIB) $(LB)$/isal.lib
# .ENDIF
APP4STDLIBS=$(SALLIB)

#------------------------------- All object files -------------------------------
# do this here, so we get right dependencies
SLOFILES=$(SHL1OBJS)

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : _cppunit.mk

