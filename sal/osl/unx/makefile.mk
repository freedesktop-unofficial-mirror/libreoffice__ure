#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.31 $
#
#   last change: $Author: obo $ $Date: 2007-01-25 15:24:24 $
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

PRJNAME=sal
.IF "$(WORK_STAMP)"=="MIX364"
TARGET=cppsal
.ELSE
TARGET=cpposl
.ENDIF
USE_LDUMP2=TRUE

PROJECTPCH4DLL=TRUE
PROJECTPCH=cont_pch
PROJECTPCHSOURCE=cont_pch

TARGETTYPE=CUI


# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

CFLAGS+= $(LFS_CFLAGS)
CXXFLAGS+= $(LFS_CFLAGS)

# --- Files --------------------------------------------------------

SLOFILES=   $(SLO)$/conditn.obj  \
            $(SLO)$/diagnose.obj \
            $(SLO)$/semaphor.obj \
            $(SLO)$/socket.obj   \
            $(SLO)$/interlck.obj \
            $(SLO)$/mutex.obj    \
            $(SLO)$/nlsupport.obj \
            $(SLO)$/thread.obj   \
            $(SLO)$/module.obj   \
            $(SLO)$/process.obj  \
            $(SLO)$/security.obj \
            $(SLO)$/profile.obj  \
            $(SLO)$/time.obj     \
            $(SLO)$/file.obj     \
            $(SLO)$/signal.obj   \
            $(SLO)$/pipe.obj   	 \
            $(SLO)$/system.obj	 \
            $(SLO)$/util.obj	 \
            $(SLO)$/tempfile.obj\
            $(SLO)$/file_url.obj\
            $(SLO)$/file_error_transl.obj\
            $(SLO)$/file_path_helper.obj\
            $(SLO)$/uunxapi.obj\
            $(SLO)$/process_impl.obj\
            $(SLO)$/file_stat.obj

#.IF "$(UPDATER)"=="YES"
OBJFILES=   $(OBJ)$/conditn.obj  \
            $(OBJ)$/diagnose.obj \
            $(OBJ)$/semaphor.obj \
            $(OBJ)$/socket.obj   \
            $(OBJ)$/interlck.obj \
            $(OBJ)$/mutex.obj    \
            $(OBJ)$/nlsupport.obj \
            $(OBJ)$/thread.obj   \
            $(OBJ)$/module.obj   \
            $(OBJ)$/process.obj  \
            $(OBJ)$/security.obj \
            $(OBJ)$/profile.obj  \
            $(OBJ)$/time.obj     \
            $(OBJ)$/file.obj     \
            $(OBJ)$/signal.obj   \
            $(OBJ)$/pipe.obj   	 \
            $(OBJ)$/system.obj	 \
            $(OBJ)$/util.obj	 \
            $(OBJ)$/tempfile.obj\
            $(OBJ)$/file_url.obj\
            $(OBJ)$/file_error_transl.obj\
            $(OBJ)$/file_path_helper.obj\
            $(OBJ)$/uunxapi.obj\
            $(OBJ)$/process_impl.obj\
            $(OBJ)$/file_stat.obj
#.ENDIF

.IF "$(OS)"=="SOLARIS" || "$(OS)"=="FREEBSD" || "$(OS)"=="NETBSD" || "$(OS)$(CPU)"=="LINUXS"
SLOFILES += $(SLO)$/backtrace.obj
OBJFILES += $(OBJ)$/backtrace.obj
.ENDIF

# --- Targets ------------------------------------------------------

.IF "$(COM)"=="C50"
APP1STDLIBS+=-lC
.ENDIF

.IF "$(OS)" == "LINUX"
.IF "$(PAM)" == "NO"
CFLAGS+=-DNOPAM
.IF "$(NEW_SHADOW_API)" == "YES"
CFLAGS+=-DNEW_SHADOW_API
.ENDIF
.ENDIF
.IF "$(PAM_LINK)" == "YES"
CFLAGS+=-DPAM_LINK
.ENDIF
.IF "$(CRYPT_LINK)" == "YES"
CFLAGS+=-DCRYPT_LINK
.ENDIF
.ENDIF

.IF "$(ENABLE_CRASHDUMP)" != "" || "$(PRODUCT)" == ""
CFLAGS+=-DSAL_ENABLE_CRASH_REPORT
.ENDIF

.INCLUDE :  target.mk

.IF "$(OS)$(CPU)"=="SOLARISS" || "$(OS)$(CPU)"=="NETBSDS" || "$(OS)$(CPU)"=="LINUXS"

$(SLO)$/interlck.obj: $(SLO)$/interlck.o
     touch $(SLO)$/interlck.obj

$(OBJ)$/interlck.obj: $(OBJ)$/interlck.o
     touch $(OBJ)$/interlck.obj

$(SLO)$/interlck.o: $(MISC)$/interlck_sparc.s
    $(ASM) $(AFLAGS) -o $@ $<

$(OBJ)$/interlck.o: $(MISC)$/interlck_sparc.s
    $(ASM) $(AFLAGS) -o $@ $<

$(MISC)$/interlck_sparc.s: asm/interlck_sparc.s
    tr -d "\015" < $< > $@

.ENDIF

.IF "$(OS)$(CPU)"=="SOLARISI"

$(SLO)$/interlck.obj: $(SLO)$/interlck.o
    touch $(SLO)$/interlck.obj

$(OBJ)$/interlck.obj: $(OBJ)$/interlck.o
    touch $(OBJ)$/interlck.obj

$(SLO)$/interlck.o: $(MISC)$/interlck_x86.s
    $(ASM) $(AFLAGS) -o $@ $<

$(OBJ)$/interlck.o: $(MISC)$/interlck_x86.s
    $(ASM) $(AFLAGS) -o $@ $<

$(MISC)$/interlck_x86.s: asm/interlck_x86.s
    tr -d "\015" < $< > $@

.ENDIF
