#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.5 $
#
#   last change: $Author: rt $ $Date: 2005-09-08 15:47:26 $
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
INCPRE+= $(PRJ)$/qa$/inc

PRJNAME=sal
TARGET=qa_rtl_ostring2

ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# BEGIN ----------------------------------------------------------------
# auto generated Target:joblist by codegen.pl 
SHL1OBJS=  \
    $(SLO)$/rtl_OString2.obj

SHL1TARGET= rtl_OString2
SHL1STDLIBS=\
   $(SALLIB) \
   $(CPPUNITLIB)

SHL1IMPLIB= i$(SHL1TARGET)
# SHL1DEF=    $(MISC)$/$(SHL1TARGET).def

DEF1NAME    =$(SHL1TARGET)
# DEF2EXPORTFILE= export.exp
SHL1VERSIONMAP= export.map
# auto generated Target:joblist
# END ------------------------------------------------------------------

# BEGIN ----------------------------------------------------------------
SHL2OBJS=  \
    $(SLO)$/rtl_str.obj

SHL2TARGET= rtl_str
SHL2STDLIBS=\
   $(SALLIB) \
   $(CPPUNITLIB)

SHL2IMPLIB= i$(SHL2TARGET)
DEF2NAME    =$(SHL2TARGET)
SHL2VERSIONMAP= export.map
# END ------------------------------------------------------------------

# BEGIN ----------------------------------------------------------------
SHL3OBJS=  \
    $(SLO)$/rtl_string.obj

SHL3TARGET= rtl_string
SHL3STDLIBS=\
   $(SALLIB) \
   $(CPPUNITLIB)

SHL3IMPLIB= i$(SHL3TARGET)
DEF3NAME    =$(SHL3TARGET)
SHL3VERSIONMAP= export.map
# END ------------------------------------------------------------------
#------------------------------- All object files -------------------------------
# do this here, so we get right dependencies
# SLOFILES=$(SHL1OBJS)

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : _cppunit.mk

