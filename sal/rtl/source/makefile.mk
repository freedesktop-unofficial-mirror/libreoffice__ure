#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2000, 2010 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

PRJ=..$/..

PRJNAME=sal
TARGET=cpprtl
ENABLE_EXCEPTIONS=TRUE
USE_LDUMP2=TRUE

PROJECTPCH4DLL=TRUE
PROJECTPCH=cont_pch
PROJECTPCHSOURCE=cont_pch

TARGETTYPE=CUI

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

.IF "$(VALGRIND_CFLAGS)" != ""
CFLAGS += $(VALGRIND_CFLAGS)
CDEFS  += -DHAVE_MEMCHECK_H=1
.ENDIF # VALGRIND_CFLAGS

.IF "$(ALLOC)" == "SYS_ALLOC" || "$(ALLOC)" == "TCMALLOC" || "$(ALLOC)" == "JEMALLOC"
CDEFS+= -DFORCE_SYSALLOC
.ENDIF

CFLAGS+= $(LFS_CFLAGS)
CXXFLAGS+= $(LFS_CFLAGS)

# --- Files --------------------------------------------------------

# safe that way: gen_makefile doesn't want it,
# no other link target here
UWINAPILIB:=

.IF "$(header)" == ""

.IF "$(OS)" != "AIX"
ALWAYSDBGFILES=$(SLO)$/debugprint.obj
.ENDIF

.IF "$(ALWAYSDBGFILES)" != ""
ALWAYSDBGTARGET=do_it_alwaysdebug
.ENDIF

SLOFILES=   \
            $(SLO)$/memory.obj      \
            $(SLO)$/cipher.obj      \
            $(SLO)$/crc.obj         \
            $(SLO)$/digest.obj      \
            $(SLO)$/random.obj      \
            $(SLO)$/locale.obj      \
            $(SLO)$/strimp.obj      \
            $(SLO)$/hash.obj        \
            $(SLO)$/string.obj      \
            $(SLO)$/ustring.obj     \
            $(SLO)$/strbuf.obj      \
            $(SLO)$/ustrbuf.obj     \
            $(SLO)$/uuid.obj        \
            $(SLO)$/rtl_process.obj \
            $(SLO)$/byteseq.obj     \
            $(SLO)$/uri.obj			\
            $(SLO)$/bootstrap.obj  	\
            $(SLO)$/cmdargs.obj		\
            $(SLO)$/unload.obj		\
            $(SLO)$/logfile.obj     \
            $(SLO)$/tres.obj        \
            $(SLO)$/debugprint.obj  \
            $(SLO)$/math.obj        \
            $(SLO)$/alloc_global.obj\
            $(SLO)$/alloc_cache.obj \
            $(SLO)$/alloc_arena.obj \
            $(SLO)$/alloc_fini.obj

OBJFILES=   \
            $(OBJ)$/memory.obj      \
            $(OBJ)$/cipher.obj      \
            $(OBJ)$/crc.obj         \
            $(OBJ)$/digest.obj      \
            $(OBJ)$/random.obj      \
            $(OBJ)$/locale.obj      \
            $(OBJ)$/strimp.obj      \
            $(OBJ)$/hash.obj        \
            $(OBJ)$/string.obj      \
            $(OBJ)$/ustring.obj     \
            $(OBJ)$/strbuf.obj      \
            $(OBJ)$/ustrbuf.obj     \
            $(OBJ)$/uuid.obj        \
            $(OBJ)$/rtl_process.obj \
            $(OBJ)$/byteseq.obj     \
            $(OBJ)$/uri.obj			\
            $(OBJ)$/bootstrap.obj  	\
            $(OBJ)$/cmdargs.obj		\
            $(OBJ)$/unload.obj		\
            $(OBJ)$/logfile.obj     \
            $(OBJ)$/tres.obj        \
            $(OBJ)$/math.obj \
            $(OBJ)$/alloc_global.obj\
            $(OBJ)$/alloc_cache.obj \
            $(OBJ)$/alloc_arena.obj \
            $(OBJ)$/alloc_fini.obj


.IF "$(CROSS_COMPILING)"==""
APP1TARGET=gen_makefile
APP1OBJS=$(SLO)$/gen_makefile.obj
APP1LIBSALCPPRT=
APP1RPATH=NONE
.ENDIF

.ENDIF

# --- Makefile snippet  --------------------------------------------

# used by e.g. update info file
BOOTSTRAPMK = $(OUT)$/inc$/rtlbootstrap.mk

# --- Targets ------------------------------------------------------

.IF "$(ALWAYSDBG_FLAG)"==""
TARGETDEPS+=$(ALWAYSDBGTARGET)
.ENDIF

.INCLUDE :  target.mk

.IF "$(ALWAYSDBGTARGET)" != ""
.IF "$(ALWAYSDBG_FLAG)" == ""
# --------------------------------------------------
# - ALWAYSDBG - files always compiled with debugging
# --------------------------------------------------
$(ALWAYSDBGTARGET):
    @echo --- ALWAYSDBGFILES ---
    @dmake $(MFLAGS) $(MAKEFILE) debug=true $(ALWAYSDBGFILES) ALWAYSDBG_FLAG=TRUE $(CALLMACROS)
    @echo --- ALWAYSDBGFILES OVER ---

$(ALWAYSDBGFILES):
    @echo --- ALWAYSDBG ---
    @dmake $(MFLAGS) $(MAKEFILE) debug=true ALWAYSDBG_FLAG=TRUE $(CALLMACROS) $@
    @echo --- ALWAYSDBG OVER ---

.ENDIF
.ENDIF


ALLTAR : $(BOOTSTRAPMK)

.IF "$(CROSS_COMPILING)"==""

$(BOOTSTRAPMK) : $(APP1TARGETN)
    $(AUGMENT_LIBRARY_PATH) $< > $@

.ELSE

$(BOOTSTRAPMK) :
    (echo '#include "macro.hxx"'; echo RTL_OS:=THIS_OS; echo RTL_ARCH:=THIS_ARCH) | $(CC) -E $(CFLAGS) $(INCLUDE_C) $(CFLAGSCC) $(CDEFS) $(CFLAGSAPPEND) - | grep '^RTL_' >$@ 

.ENDIF
