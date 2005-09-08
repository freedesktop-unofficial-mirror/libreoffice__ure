#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.11 $
#
#   last change: $Author: rt $ $Date: 2005-09-08 16:50:54 $
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

# remove this, when issue i35064 is integrated
.IF "$(COM)"=="GCC"
NOOPTFILES= \
    $(SLO)$/pyuno_module.obj
.ENDIF			# "$(COM)"=="GCC"


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
    @+ echo $(LINK) $(LINKFLAGS) $(LINKFLAGSSHLCUI) -ldl -o $@ $(SLO)$/pyuno_dlopenwrapper.o > $(MISC)$/$(@:b).cmd
.ELIF "$(OS)" == "SOLARIS"
    @+ echo ld -G -ldl -o $@ $(SLO)$/pyuno_dlopenwrapper.o > $(MISC)$/$(@:b).cmd
.ELIF "$(OS)" == "FREEBSD"
    @+ echo ld -shared -o $@ $(SLO)$/pyuno_dlopenwrapper.o > $(MISC)$/$(@:b).cmd
.ELIF "$(OS)" == "NETBSD"
    @+ echo $(LINK) $(LINKFLAGSSHLCUI) -o $@ $(SLO)$/pyuno_dlopenwrapper.o > $(MISC)$/$(@:b).cmd
.ELIF "$(OS)" == "MACOSX"
    @+ echo $(CC) -dynamiclib -ldl -o $@ $(SLO)$/pyuno_dlopenwrapper.o > $(MISC)$/$(@:b).cmd
.ELSE
    @+ echo $(LINK) $(LINKFLAGSSHLCUI) -o $@ $(SLO)$/pyuno_dlopenwrapper.o > $(MISC)$/$(@:b).cmd
.ENDIF
    cat $(MISC)$/$(@:b).cmd
    @source $(MISC)$/$(@:b).cmd
.ENDIF


$(MISC)$/$(PYUNORC) : pyuno
    -rm -f $@
    cat pyuno > $@ 

$(MISC)$/pyuno.flt : pyuno.flt
    -rm -f $@
    cat $? > $@

