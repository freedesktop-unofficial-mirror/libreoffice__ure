#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.1 $
#
#   last change: $Author: jl $ $Date: 2002-07-23 14:02:26 $
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
PRJ=..$/..$/..
TARGET= jvminteraction
PRJNAME=	stoc
TARGET6=	$(TARGET)
TARGETTYPE=	CUI
LIBTARGET=	NO
ENABLE_EXCEPTIONS=TRUE
NO_BSYMBOLIC=	TRUE

UNOUCRDEP=	$(SOLARBINDIR)$/applicat.rdb
UNOUCRRDB=	$(SOLARBINDIR)$/applicat.rdb
UNOUCROUT=	$(OUT)$/inc$
INCPRE+=	$(OUT)$/inc$



# --- Settings -----------------------------------------------------
.INCLUDE :  settings.mk

# --- Application 6 - testjavavm ------------------------------------

UNOTYPES= com.sun.star.lang.XMultiComponentFactory \
    com.sun.star.uno.XWeak	\
    com.sun.star.java.XJavaVM	\
    com.sun.star.java.XJavaThreadRegister_11	\
    com.sun.star.java.JavaNotConfiguredException \
    com.sun.star.java.MissingJavaRuntimeException \
    com.sun.star.java.JavaDisabledException \
    com.sun.star.java.JavaVMCreationFailureException \
    com.sun.star.registry.XSimpleRegistry \
    com.sun.star.lang.XComponent \
    com.sun.star.registry.XImplementationRegistration \
    com.sun.star.lang.XSingleServiceFactory \
    com.sun.star.uno.TypeClass \
    com.sun.star.lang.XMultiServiceFactory \
    com.sun.star.uno.XCurrentContext \
    com.sun.star.task.XInteractionHandler \
    com.sun.star.task.XInteractionRequest \
    com.sun.star.task.XInteractionContinuation \
    com.sun.star.task.XInteractionAbort \
    com.sun.star.task.XInteractionRetry \

    
APP6TARGET= 	$(TARGET6)
APP6OBJS  = 	$(OBJ)$/interactionhandler.obj 
APP6STDLIBS= \
        $(CPPULIB) 		\
        $(CPPUHELPERLIB) 	\
        $(SALHELPERLIB) 	\
        $(SALLIB)	
#		$(UNOLIB)

.IF "$(GUI)"=="WNT"
APP6STDLIBS+=	$(LIBCIMT)
.ENDIF

#ALLIDLFILES:=

# --- Target ------------------------------------------------

.IF "$(depend)" == ""
#ALL : 		unoheader	\
#		ALLTAR
ALL : 		ALLTAR
.ELSE
ALL: 		ALLDEP
.ENDIF

.INCLUDE :	target.mk

