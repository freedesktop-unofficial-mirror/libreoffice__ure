#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.4 $
#
#   last change: $Author: hr $ $Date: 2000-11-13 16:55:30 $
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

PRJNAME=bridges
TARGET=javaloader
ENABLE_EXCEPTIONS=TRUE
USE_DEFFILE=TRUE
NO_BSYMBOLIC=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# ------------------------------------------------------------------

CPPUMAKERFLAGS += -C
UNOUCRDEP=$(SOLARBINDIR)$/udkapi.rdb
UNOUCRRDB=$(SOLARBINDIR)$/udkapi.rdb 

# output directory (one dir for each project)
UNOUCROUT=$(OUT)$/inc

# adding to inludepath
INCPRE+=$(UNOUCROUT)

UNOTYPES= \
    com.sun.star.java.XJavaThreadRegister_11	\
    com.sun.star.java.XJavaVM					\
    com.sun.star.lang.IllegalArgumentException	\
    com.sun.star.lang.XInitialization			\
    com.sun.star.lang.XMultiServiceFactory		\
    com.sun.star.lang.XServiceInfo				\
    com.sun.star.lang.XSingleServiceFactory		\
    com.sun.star.lang.XTypeProvider				\
    com.sun.star.loader.XImplementationLoader	\
    com.sun.star.registry.XRegistryKey			\
    com.sun.star.uno.TypeClass					\
    com.sun.star.uno.XAggregation				\
    com.sun.star.uno.XWeak

SLOFILES= 	$(SLO)$/javaloader.obj

SHL1TARGET= $(TARGET)

SHL1STDLIBS=\
        $(CPPUHELPERLIB) 	\
        $(CPPULIB)			\
        $(VOSLIB)			\
        $(SALLIB)


SHL1LIBS=\
            $(SLB)$/$(TARGET).lib 


# --- Targets ------------------------------------------------------

.INCLUDE :	target.mk


$(MISC)$/$(SHL1TARGET).def: makefile.mk
    @echo ------------------------------
    @echo Making: $@
    @echo LIBRARY     $(SHL1TARGET)                 >$@
    @echo EXPORTS                                   >>$@
    @echo component_getImplementationEnvironment    >>$@
    @echo component_writeInfo                       >>$@
    @echo component_getFactory					    >>$@

