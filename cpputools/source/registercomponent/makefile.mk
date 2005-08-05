#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.14 $
#
#   last change: $Author: hr $ $Date: 2005-08-05 12:57:37 $
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

PRJNAME=cpputools
TARGET=regcomp
LIBTARGET=NO

ENABLE_EXCEPTIONS=TRUE

.IF "$(OS)" == "LINUX"
LINKFLAGSRUNPATH = -Wl,-rpath,\''$$ORIGIN/../lib:$$ORIGIN'\'
.ELIF "$(OS)" == "SOLARIS"
LINKFLAGSRUNPATH = -R\''$$ORIGIN/../lib:$$ORIGIN'\'
.ENDIF

# --- Settings -----------------------------------------------------
.INCLUDE :  settings.mk

UNOUCRDEP=$(SOLARBINDIR)$/udkapi.rdb 
UNOUCRRDB=$(SOLARBINDIR)$/udkapi.rdb

UNOUCROUT = $(OUT)$/inc$/$(TARGET)
INCPRE += $(UNOUCROUT)
CPPUMAKERFLAGS += -C

UNIXTEXT= $(MISC)$/regcomp.sh
UNOTYPES=\
             com.sun.star.uno.TypeClass \
             com.sun.star.lang.XMultiServiceFactory \
             com.sun.star.lang.XSingleServiceFactory \
             com.sun.star.lang.XMultiComponentFactory \
             com.sun.star.lang.XSingleComponentFactory \
             com.sun.star.lang.XComponent \
             com.sun.star.container.XContentEnumerationAccess \
             com.sun.star.container.XSet \
             com.sun.star.loader.CannotActivateFactoryException \
            com.sun.star.registry.XImplementationRegistration

# --- Files --------------------------------------------------------
CDEFS += -DDLL_VERSION=\"$(UPD)$(DLLPOSTFIX)\"

DEPOBJFILES=   $(OBJ)$/registercomponent.obj 

APP1TARGET= $(TARGET)
APP1OBJS=$(DEPOBJFILES)  

APP1STDLIBS=\
            $(SALLIB) \
            $(CPPULIB)	\
            $(CPPUHELPERLIB)

.IF "$(GUI)"=="WNT"
APP1STDLIBS+= \
            $(LIBCMT)
.ENDIF


.INCLUDE :  target.mk


.IF "$(GUI)"=="UNX"
ALLTAR: REGCOMPSH

REGCOMPSH : $(UNIXTEXT)
    +-chmod +x $(UNIXTEXT)
.ENDIF	
