#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.17 $
#
#   last change: $Author: obo $ $Date: 2004-06-01 09:04:58 $
#
#   The Contents of this file are made available subject to the terms of
#   either of the following licenses
#
#          - GNU Lesser General Public License Version 2.1
#          - Sun Industry Standards Source License Version 1.1
#
#   Sun Microsystems Inc., October, 2000
#
#   GNU Lesser General Public License Version 2.1
#   =============================================
#   Copyright 2000 by Sun Microsystems, Inc.
#   901 San Antonio Road, Palo Alto, CA 94303, USA
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Lesser General Public
#   License version 2.1, as published by the Free Software Foundation.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public
#   License along with this library; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#   MA  02111-1307  USA
#
#
#   Sun Industry Standards Source License Version 1.1
#   =================================================
#   The contents of this file are subject to the Sun Industry Standards
#   Source License Version 1.1 (the "License"); You may not use this file
#   except in compliance with the License. You may obtain a copy of the
#   License at http://www.openoffice.org/license.html.
#
#   Software provided under this License is provided on an "AS IS" basis,
#   WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
#   WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
#   MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
#   See the License for the specific provisions governing your rights and
#   obligations concerning the Software.
#
#   The Initial Developer of the Original Code is: Sun Microsystems, Inc.
#
#   Copyright: 2000 by Sun Microsystems, Inc.
#
#   All Rights Reserved.
#
#   Contributor(s): _______________________________________
#
#
#
#*************************************************************************
PRJ=..$/..

PRJNAME=stoc
TARGET = javavm.uno
ENABLE_EXCEPTIONS=TRUE
NO_BSYMBOLIC = TRUE
COMP1TYPELIST = jen

.IF "$(SOLAR_JAVA)" == "TRUE"
# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
DLLPRE =

# --- Files --------------------------------------------------------

# Kollision zwischen bool.h aus Java und bool.h aus der STL.
# Das Problem tritt fuer alle Plattformen auf, aber anscheinend stolpert nur der
# GCC darueber
.IF "$(COM)" == "GCC"
CDEFS += -D__SGI_STL_BOOL_H
.ENDIF

.INCLUDE :  ..$/cppumaker.mk

SLOFILES= \
        $(SLO)$/javavm.obj		\
        $(SLO)$/jvmargs.obj	\
        $(SLO)$/interact.obj 

.IF "$(COM)"=="C52"
# Code generation bug in conjunction with exception handling
# You can find a codesample in javavm.cxx in a comment
# at the top of the file
NOOPTFILES= \
        $(SLO)$/javavm.obj
.ENDIF			# "$(COM)"=="C52"

SHL1TARGET= $(TARGET)
SHL1VERSIONMAP = jen.map
SHL1STDLIBS= \
        $(CPPUHELPERLIB) 	\
        $(CPPULIB)	    	\
        $(UNOLIB)	    	\
        $(SALLIB) \
        $(JVMACCESSLIB) \
        $(SALHELPERLIB) \
    $(JVMFWKLIB)

.IF "$(GUI)"=="WNT"
SHL1STDLIBS += advapi32.lib
.ENDIF

SHL1DEPN=		
SHL1IMPLIB=		i$(TARGET)
SHL1LIBS=		$(SLB)$/$(TARGET).lib
SHL1DEF=		$(MISC)$/$(SHL1TARGET).def

DEF1NAME=		$(SHL1TARGET)

.ELSE		# SOLAR_JAVA
all:
    @echo Nothing to do: SOLAR_JAVA not set
.ENDIF

# --- Targets ------------------------------------------------------


.INCLUDE :	target.mk

