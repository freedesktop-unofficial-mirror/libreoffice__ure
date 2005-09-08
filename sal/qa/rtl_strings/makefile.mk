#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.8 $
#
#   last change: $Author: rt $ $Date: 2005-09-08 15:54:55 $
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
PRJ=..$/..

PRJNAME=sal
TARGET=qa_rtl_strings
# TESTDIR=TRUE

ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# BEGIN ----------------------------------------------------------------
# auto generated Target:FileBase by codegen.pl 
SHL1OBJS=  \
    $(SLO)$/rtl_String_Utils.obj \
    $(SLO)$/rtl_OString.obj

SHL1TARGET= rtl_OString
SHL1STDLIBS=\
   $(SALLIB) \
   $(CPPUNITLIB)

SHL1IMPLIB= i$(SHL1TARGET)
# SHL1DEF=    $(MISC)$/$(SHL1TARGET).def

DEF1NAME    =$(SHL1TARGET)
# DEF1EXPORTFILE= export.exp
SHL1VERSIONMAP = export.map

# auto generated Target:FileBase
# END ------------------------------------------------------------------

# BEGIN ----------------------------------------------------------------
# auto generated Target:FileBase by codegen.pl 
SHL2OBJS=  \
    $(SLO)$/rtl_String_Utils.obj \
    $(SLO)$/rtl_OUString.obj

SHL2TARGET= rtl_OUString
SHL2STDLIBS=\
   $(SALLIB) \
   $(CPPUNITLIB)

SHL2IMPLIB= i$(SHL2TARGET)
# SHL2DEF=    $(MISC)$/$(SHL2TARGET).def

DEF2NAME    =$(SHL2TARGET)
# DEF2EXPORTFILE= export.exp
SHL2VERSIONMAP = export.map

# auto generated Target:FileBase
# END ------------------------------------------------------------------

# BEGIN ----------------------------------------------------------------
# auto generated Target:FileBase by codegen.pl 
SHL3OBJS=  \
    $(SLO)$/rtl_String_Utils.obj \
    $(SLO)$/rtl_OUStringBuffer.obj

SHL3TARGET= rtl_OUStringBuffer
SHL3STDLIBS=\
   $(SALLIB) \
   $(CPPUNITLIB)

# .IF "$(GUI)" == "WNT"
# SHL3STDLIBS+=	$(SOLARLIBDIR)$/cppunit.lib
# .ENDIF
# .IF "$(GUI)" == "UNX"
# SHL3STDLIBS+=$(SOLARLIBDIR)$/libcppunit$(DLLPOSTFIX).a
# .ENDIF

SHL3IMPLIB= i$(SHL3TARGET)
# SHL3DEF=    $(MISC)$/$(SHL3TARGET).def

DEF3NAME    =$(SHL3TARGET)
# DEF3EXPORTFILE= export.exp
SHL3VERSIONMAP = export.map

# auto generated Target:FileBase
# END ------------------------------------------------------------------

# BEGIN ----------------------------------------------------------------
SHL4OBJS=  \
    $(SLO)$/rtl_old_teststrbuf.obj 

SHL4TARGET= rtl_old_teststrbuf
SHL4STDLIBS=\
   $(SALLIB) \
   $(CPPUNITLIB)

SHL4IMPLIB= i$(SHL4TARGET)
DEF4NAME    =$(SHL4TARGET)
SHL4VERSIONMAP = export.map

# END ------------------------------------------------------------------

# BEGIN ----------------------------------------------------------------
SHL5OBJS=  \
    $(SLO)$/rtl_old_testowstring.obj 

SHL5TARGET= rtl_old_testowstring
SHL5STDLIBS=\
   $(SALLIB) \
   $(CPPUNITLIB)

SHL5IMPLIB= i$(SHL5TARGET)
DEF5NAME    =$(SHL5TARGET)
SHL5VERSIONMAP = export.map

# END ------------------------------------------------------------------

# BEGIN ----------------------------------------------------------------
SHL6OBJS=  \
    $(SLO)$/rtl_old_testostring.obj 

SHL6TARGET= rtl_old_testostring
SHL6STDLIBS=\
   $(SALLIB) \
   $(CPPUNITLIB)

SHL6IMPLIB= i$(SHL6TARGET)
DEF6NAME    =$(SHL6TARGET)
SHL6VERSIONMAP = export.map

# END ------------------------------------------------------------------

#------------------------------- All object files -------------------------------
# do this here, so we get right dependencies
SLOFILES=\
    $(SHL1OBJS) \
    $(SHL2OBJS) \
    $(SHL3OBJS) \
    $(SHL4OBJS) \
    $(SHL5OBJS) \
    $(SHL6OBJS)

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : _cppunit.mk


