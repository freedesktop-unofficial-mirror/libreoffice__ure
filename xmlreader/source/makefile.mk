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
#***********************************************************************/

PRJ = ..
PRJNAME = xmlreader
TARGET = xmlreader

ENABLE_EXCEPTIONS = TRUE
VISIBILITY_HIDDEN = TRUE

.INCLUDE: settings.mk

CDEFS += -DOOO_DLLIMPLEMENTATION_XMLREADER

SLOFILES = \
    $(SLO)/pad.obj \
    $(SLO)/span.obj \
    $(SLO)/xmlreader.obj

.IF "$(COM)" == "MSC"
SHL1IMPLIB = i$(SHL1TARGET)
.ELSE
SHL1IMPLIB= $(SHL1TARGET)
.ENDIF
SHL1OBJS = $(SLOFILES)
SHL1RPATH = URELIB
SHL1STDLIBS = \
    $(SALLIB)
SHL1TARGET = xmlreader
SHL1USE_EXPORTS = name
DEF1NAME = $(SHL1TARGET)

.INCLUDE: target.mk
