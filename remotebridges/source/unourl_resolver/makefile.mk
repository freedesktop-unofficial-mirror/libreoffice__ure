#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.9 $
#
#   last change: $Author: rt $ $Date: 2005-09-09 05:24:04 $
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

PRJNAME=remotebridges
TARGET = uuresolver.uno
NO_BSYMBOLIC=TRUE
ENABLE_EXCEPTIONS=TRUE
COMP1TYPELIST = uuresolver

# --- Settings -----------------------------------------------------

.INCLUDE :  svpre.mk
.INCLUDE :  settings.mk
DLLPRE =
.INCLUDE :  sv.mk

# ------------------------------------------------------------------
UNOUCRDEP=$(SOLARBINDIR)$/udkapi.rdb
UNOUCRRDB=$(SOLARBINDIR)$/udkapi.rdb
UNOUCROUT=$(OUT)$/inc$/uuresolver
INCPRE+= $(UNOUCROUT)


UNOTYPES= \
        com.sun.star.uno.TypeClass		\
        com.sun.star.lang.XTypeProvider		\
        com.sun.star.lang.XServiceInfo		\
        com.sun.star.lang.XSingleServiceFactory	\
        com.sun.star.lang.XMultiServiceFactory	\
        com.sun.star.lang.XComponent		\
        com.sun.star.registry.XRegistryKey	\
        com.sun.star.connection.XConnector	\
        com.sun.star.bridge.XBridgeFactory	\
        com.sun.star.bridge.XUnoUrlResolver	\
        com.sun.star.uno.XAggregation		\
        com.sun.star.uno.XWeak                  

SLOFILES= \
        $(SLO)$/unourl_resolver.obj

SHL1TARGET=	$(TARGET)
SHL1VERSIONMAP = uuresolver.map

SHL1STDLIBS= \
        $(SALLIB)		\
        $(CPPULIB)		\
        $(CPPUHELPERLIB)

SHL1DEPN=
SHL1IMPLIB=	i$(TARGET)
SHL1LIBS=	$(SLB)$/$(TARGET).lib
SHL1DEF=	$(MISC)$/$(SHL1TARGET).def

DEF1NAME=	$(SHL1TARGET)

# --- Targets ------------------------------------------------------

.INCLUDE :	target.mk
