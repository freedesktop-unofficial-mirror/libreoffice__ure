#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.7 $
#
#   last change: $Author: rt $ $Date: 2004-09-08 16:52:36 $
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
#   Contributor(s): Ralph Thomas, Joerg Budischewski
#
#*************************************************************************
PRJ=..$/..

PRJNAME=pyuno
TARGET=pyuno
ENABLE_EXCEPTIONS=TRUE

LINKFLAGSDEFS = # do not fail with missing symbols

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
#-------------------------------------------------------------------

.IF "$(OS)$(CPU)$(COMEX)" == "SOLARISS4"
# no -Bdirect for SunWS CC
DIRECT = $(LINKFLAGSDEFS)
.ENDIF

.IF "$(GUI)" == "UNX"
# python expects modules without the lib prefix 
PYUNO_MODULE=$(DLLDEST)$/pyuno$(DLLPOST)
PYUNORC=pyunorc
.ELSE
.INCLUDE :  pyversion.mk
PYUNORC=pyuno.ini
.ENDIF

.IF "$(SYSTEM_PYTHON)" == "YES"
PYTHONLIB=$(PYTHON_LIBS)
CFLAGS+=$(PYTHON_CFLAGS)
.ELSE # "$(SYSTEM_PYTHON)" == "YES"
.INCLUDE :  pyversion.mk
.IF "$(GUI)" == "UNX"
# python executable brings in the needed python core symbols,
# so this library cannot be checked
SHL1NOCHECK=yes
.IF "$(OS)"=="SOLARIS" || "$(OS)"=="MACOSX"
PYTHONLIB=-lpython
.ENDIF # "$(OS)"=="SOLARIS" || "$(OS)"=="MACOSX"
.ELSE # "$(GUI)" == "UNX"
# on windows, the python executable also uses the shared library,
# so we link pyuno directly to it
PYTHONLIB=python$(PYMAJOR)$(PYMINOR).lib
.ENDIF # "$(GUI)" == "UNX"
CFLAGS+=-I$(SOLARINCDIR)$/python
.ENDIF # "$(SYSTEM_PYTHON)" == "YES"

SHL1TARGET=$(TARGET)
SLOFILES= \
        $(SLO)$/pyuno_runtime.obj 	\
        $(SLO)$/pyuno.obj 		\
        $(SLO)$/pyuno_callable.obj 	\
        $(SLO)$/pyuno_module.obj 	\
        $(SLO)$/pyuno_type.obj 		\
        $(SLO)$/pyuno_util.obj		\
        $(SLO)$/pyuno_except.obj	\
        $(SLO)$/pyuno_adapter.obj	\
        $(SLO)$/pyuno_gc.obj

SHL1STDLIBS= \
        $(CPPULIB)		\
        $(CPPUHELPERLIB)	\
        $(SALLIB)		\
        $(PYTHONLIB)

SHL1DEPN=
SHL1LIBS=$(SLB)$/$(TARGET).lib
SHL1IMPLIB=i$(TARGET)

SHL1DEF=	$(MISC)$/$(SHL1TARGET).def

DEF1NAME=	$(SHL1TARGET)
DEF1DEPN=	$(MISC)$/pyuno.flt

DEFLIB1NAME=$(TARGET)

# --- Targets ------------------------------------------------------

ALLTAR : \
    $(DLLDEST)$/uno.py 		\
    $(DLLDEST)$/unohelper.py	\
    $(PYUNO_MODULE)			\
    $(MISC)$/$(PYUNORC)		

.INCLUDE :  target.mk

$(DLLDEST)$/%.py: %.py
    +cp $? $@


.IF "$(GUI)" == "UNX"
$(PYUNO_MODULE) : $(SLO)$/pyuno_dlopenwrapper.obj
.IF "$(OS)" == "LINUX"
    ld -shared -ldl -o $@ $(SLO)$/pyuno_dlopenwrapper.o
.ELIF "$(OS)" == "SOLARIS"
    ld -G -ldl -o $@ $(SLO)$/pyuno_dlopenwrapper.o
.ELIF "$(OS)" == "FREEBSD"
    ld -shared -o $@ $(SLO)$/pyuno_dlopenwrapper.o
.ELIF "$(OS)" == "NETBSD"
    $(LINK) $(LINKFLAGSSHLCUI) -o $@ $(SLO)$/pyuno_dlopenwrapper.o
.ELIF "$(OS)" == "MACOSX"
    $(CC) -dynamiclib -ldl -o $@ $(SLO)$/pyuno_dlopenwrapper.o
.ENDIF
.ENDIF


$(MISC)$/$(PYUNORC) : pyuno
    -rm -f $@
    cat pyuno > $@ 

$(MISC)$/pyuno.flt : pyuno.flt
    -rm -f $@
    cat $? > $@
