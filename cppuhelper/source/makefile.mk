#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.10 $
#
#   last change: $Author: jsc $ $Date: 2001-05-04 13:38:30 $
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
PRJ=..

PRJNAME=	cppuhelper
TARGET=		cppuhelper
NO_BSYMBOLIC=	TRUE
ENABLE_EXCEPTIONS=TRUE

USE_DEFFILE=	TRUE

# --- Settings -----------------------------------------------------
.INCLUDE :  settings.mk

# --- Files --------------------------------------------------------

UNOUCRDEP=	$(SOLARBINDIR)$/udkapi.rdb
UNOUCRRDB=	$(SOLARBINDIR)$/udkapi.rdb
UNOUCROUT=	$(OUT)$/inc
INCPRE+=	$(OUT)$/inc

CPPUMAKERFLAGS =

UNOTYPES= \
        com.sun.star.registry.XSimpleRegistry		\
        com.sun.star.registry.XImplementationRegistration \
        com.sun.star.container.XSet			\
        com.sun.star.uno.XWeak 				\
        com.sun.star.uno.XAggregation 			\
        com.sun.star.lang.XMultiServiceFactory 		\
        com.sun.star.lang.XSingleServiceFactory 	\
        com.sun.star.lang.XServiceInfo 			\
        com.sun.star.lang.XInitialization 		\
        com.sun.star.reflection.XIdlReflection 		\
        com.sun.star.reflection.XIdlClass 		\
        com.sun.star.reflection.XIdlClassProvider	\
        com.sun.star.reflection.XIndirectTypeDescription	\
        com.sun.star.reflection.XMethodParameter	\
        com.sun.star.reflection.XInterfaceMemberTypeDescription	\
        com.sun.star.beans.XPropertySet 		\
        com.sun.star.beans.XMultiPropertySet 		\
        com.sun.star.beans.XFastPropertySet 		\
        com.sun.star.beans.PropertyAttribute 		\
        com.sun.star.registry.XRegistryKey		\
        com.sun.star.loader.XImplementationLoader	\
        com.sun.star.lang.XTypeProvider			\
        com.sun.star.lang.XComponent

SLOFILES=	\
        $(SLO)$/typeprovider.obj 	\
        $(SLO)$/exc_thrower.obj 	\
        $(SLO)$/servicefactory.obj 	\
        $(SLO)$/implbase.obj 		\
        $(SLO)$/propshlp.obj 		\
        $(SLO)$/weak.obj		\
        $(SLO)$/interfacecontainer.obj	\
        $(SLO)$/stdidlclass.obj 	\
        $(SLO)$/factory.obj		\
        $(SLO)$/component.obj		\
        $(SLO)$/shlib.obj

SHL1TARGET=	$(TARGET)$(UDK_MAJOR)$(COM)

SHL1STDLIBS= \
        $(CPPULIB)		\
        $(SALLIB)

SHL1DEPN=
SHL1IMPLIB=	i$(TARGET)
SHL1LIBS=	$(SLB)$/$(TARGET).lib
SHL1DEF=	$(MISC)$/$(SHL1TARGET).def

DEF1NAME=	$(SHL1TARGET)
DEF1DEPN=	$(MISC)$/$(SHL1TARGET).flt
DEFLIB1NAME=	$(TARGET)

# --- Targets ------------------------------------------------------

.INCLUDE :	target.mk

$(MISC)$/$(SHL1TARGET).flt : makefile.mk
    +echo	_TI2	   >$@
    +echo	_TI1	  >>$@
