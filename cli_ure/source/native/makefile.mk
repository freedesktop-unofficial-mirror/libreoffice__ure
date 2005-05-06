#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.10 $
#
#   last change: $Author: obo $ $Date: 2005-05-06 09:41:01 $
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

PRJ = ..$/..
PRJNAME = cli_ure

TARGET = cli_cppuhelper
NO_BSYMBOLIC = TRUE
ENABLE_EXCEPTIONS = TRUE
LIBTARGET = NO
USE_DEFFILE = TRUE

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/util$/makefile.pmk

use_shl_versions=

.IF "$(USE_SHELL)"!="4nt"
ECHOQUOTE='
.ELSE
ECHOQUOTE=
.ENDIF

.IF "$(BUILD_FOR_CLI)" != ""

#!!! Always change version if code has changed. Provide a publisher
#policy assembly!!!
ASSEMBLY_VERSION="1.0.0.0"

ASSEMBLY_KEY="$(BIN)$/cliuno.snk"

ASSEMBLY_ATTRIBUTES = $(MISC)$/assembly_cppuhelper.cxx

ALLTAR : \
    $(ASSEMBLY_ATTRIBUTES)

ASSEMBLY_KEY_X=$(subst,\,\\ $(ASSEMBLY_KEY)) 

$(ASSEMBLY_ATTRIBUTES) .PHONY:
    +echo $(ASSEMBLY_KEY_X)
    $(GNUCOPY) -p assembly.cxx $@
    +echo $(ECHOQUOTE) \
    [assembly:System::Reflection::AssemblyVersion( $(ASSEMBLY_VERSION) )]; $(ECHOQUOTE) \
    >> $(OUT)$/misc$/assembly_cppuhelper.cxx	
    +echo $(ECHOQUOTE) \
    [assembly:System::Reflection::AssemblyKeyFile($(ASSEMBLY_KEY_X))]; $(ECHOQUOTE) \
    >> $(OUT)$/misc$/assembly_cppuhelper.cxx


LINKFLAGS += /delayload:cppuhelper3MSC.dll \
             /delayload:cppu3.dll \
             /delayload:sal3.dll

UWINAPILIB=

UNOUCRRDB = $(SOLARBINDIR)$/udkapi.rdb
UNOUCRDEP = $(UNOUCRRDB)
UNOUCROUT = $(OUT)$/inc
INCPRE += $(OUT)$/inc

CPPUMAKERFLAGS =
UNOTYPES = \
    com.sun.star.lang.XSingleComponentFactory			\
    com.sun.star.loader.CannotActivateFactoryException 		\
    com.sun.star.registry.CannotRegisterImplementationException	\
    com.sun.star.registry.XRegistryKey

CFLAGS += -clr -AI $(OUT)$/bin
#see  Microsoft Knowledge Base Article - 814472 
LINKFLAGS += -NOENTRY -NODEFAULTLIB:nochkclr.obj -INCLUDE:__DllMainCRTStartup@12

SLOFILES = \
        $(SLO)$/native_bootstrap.obj \
    $(SLO)$/assembly_cppuhelper.obj 

SHL1OBJS = $(SLOFILES)

SHL1TARGET = $(TARGET)

SHL1STDLIBS = \
    $(CPPUHELPERLIB) \
    $(CPPULIB)		\
    $(SALLIB)		\
    delayimp.lib \
    advapi32.lib \
    mscoree.lib \
    Advapi32.lib

SHL1VERSIONMAP = msvc.map

SHL1DEF = $(MISC)$/$(SHL1TARGET).def
DEF1NAME = $(SHL1TARGET)

.ENDIF

.INCLUDE : $(PRJ)$/util$/target.pmk
.INCLUDE : target.mk
