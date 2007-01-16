#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.10 $
#
#   last change: $Author: vg $ $Date: 2007-01-16 15:56:17 $
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
TARGET=qa_osl_file

ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

CFLAGS+= $(LFS_CFLAGS)
CXXFLAGS+= $(LFS_CFLAGS)

# --- BEGIN --------------------------------------------------------
SHL1OBJS=  \
    $(SLO)$/osl_File.obj
SHL1TARGET= osl_File
SHL1STDLIBS=\
    $(SALLIB) \
    $(CPPUNITLIB)
# .IF "$(GUI)" == "WNT"
# SHL1STDLIBS+=	$(SOLARLIBDIR)$/cppunit.lib
# .ENDIF
# .IF "$(GUI)" == "UNX"
# SHL1STDLIBS+=$(SOLARLIBDIR)$/libcppunit$(DLLPOSTFIX).a
# .ENDIF

SHL1IMPLIB= i$(SHL1TARGET)

DEF1NAME    =$(SHL1TARGET)
SHL1VERSIONMAP = export.map

#-------------------------------------------------------------------

SHL2OBJS=$(SLO)$/test_cpy_wrt_file.obj
SHL2TARGET=tcwf
SHL2STDLIBS=$(SALLIB)
.IF "$(GUI)"=="WNT"
SHL2STDLIBS+=$(SOLARLIBDIR)$/cppunit.lib
.ENDIF
.IF "$(GUI)" == "UNX"
SHL2STDLIBS+=$(SOLARLIBDIR)$/libcppunit$(DLLPOSTFIX).a
.ENDIF
SHL2IMPLIB=i$(SHL2TARGET)
SHL2DEF=$(MISC)$/$(SHL2TARGET).def
SHL2VERSIONMAP = export.map
DEF2NAME    =$(SHL2TARGET)


# END --------------------------------------------------------------

# --- BEGIN --------------------------------------------------------
SHL3OBJS=  \
    $(SLO)$/osl_old_test_file.obj
SHL3TARGET= osl_old_test_file
SHL3STDLIBS=\
    $(SALLIB) \
    $(CPPUNITLIB)

SHL3IMPLIB= i$(SHL3TARGET)

DEF3NAME    =$(SHL3TARGET)
SHL3VERSIONMAP = export.map
# END --------------------------------------------------------------

#------------------------------- All object files -------------------------------
# do this here, so we get right dependencies
SLOFILES=$(SHL1OBJS)

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : _cppunit.mk
