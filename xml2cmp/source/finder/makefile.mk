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
# $Revision: 1.13 $
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

PRJ=..$/..

PRJNAME=xml2cmp
TARGET=srvdepy
TARGETTYPE=CUI

# --- Settings -----------------------------------------------------

ENABLE_EXCEPTIONS=TRUE

.INCLUDE :  settings.mk



# --- Files --------------------------------------------------------

OBJFILES= \
    $(OBJ)$/dependy.obj	\
    $(OBJ)$/dep_main.obj


# --- Targets ------------------------------------------------------

APP1TARGET=	$(TARGET)
.IF "$(GUI)"!="OS2"
APP1STACK=	1000000
.ENDIF

LIBSALCPPRT=$(0)

.IF "$(GUI)"=="WNT"
UWINAPILIB=$(0)
.ENDIF


APP1LIBS=	$(LB)$/$(TARGET).lib $(LB)$/x2c_xcdl.lib $(LB)$/x2c_support.lib
APP1DEPN=	$(LB)$/$(TARGET).lib $(LB)$/x2c_xcdl.lib $(LB)$/x2c_support.lib


.INCLUDE :  target.mk


