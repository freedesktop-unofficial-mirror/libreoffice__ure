#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.9 $
#
#   last change: $Author: rt $ $Date: 2004-08-20 14:11:13 $
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
PRJNAME=sal
TARGET=uwinapi

USE_DEFFILE=TRUE
LIBTARGET=NO

# --- Settings ----------------------------------

.INCLUDE : settings.mk

# --- Targets ----------------------------------


.IF "$(GUI)"=="WNT"

.IF "$(COMEX)"=="9"
.IF "$(PSDK_HOME)"!=""
# Since the 02/2003 PSDK the "new" linker is needed here.
LINK=$(WRAPCMD) "$(PSDK_HOME)$/Bin$/Win64$/LINK.EXE"
.ENDIF
.ENDIF

SLOFILES=\
        $(SLO)$/CommandLineToArgvW.obj\
        $(SLO)$/CopyFileExA.obj\
        $(SLO)$/CopyFileExW.obj\
        $(SLO)$/DrawStateW.obj\
        $(SLO)$/EnumProcesses.obj\
        $(SLO)$/GetLogicalDriveStringsW.obj\
        $(SLO)$/GetLongPathNameA.obj\
        $(SLO)$/GetLongPathNameW.obj\
        $(SLO)$/GetModuleFileNameExA.obj\
        $(SLO)$/GetModuleFileNameExW.obj\
        $(SLO)$/GetProcessId.obj\
        $(SLO)$/GetUserDefaultUILanguage.obj\
        $(SLO)$/GetUserDomainA.obj\
        $(SLO)$/GetUserDomainW.obj\
        $(SLO)$/GetDiskFreeSpaceExA.obj\
        $(SLO)$/GetDiskFreeSpaceExW.obj\
        $(SLO)$/MoveFileExA.obj\
        $(SLO)$/MoveFileExW.obj\
        $(SLO)$/toolhelp.obj\
        $(SLO)$/DllGetVersion.obj\
        $(SLO)$/DllMain.obj\
        $(SLO)$/ResolveThunk.obj\
        $(SLO)$/snprintf.obj\
        $(SLO)$/snwprintf.obj\
        $(SLO)$/FindFirstVolumeA.obj\
        $(SLO)$/FindFirstVolumeW.obj\
        $(SLO)$/FindNextVolumeA.obj\
        $(SLO)$/FindNextVolumeW.obj\
        $(SLO)$/FindVolumeClose.obj\
        $(SLO)$/FindFirstVolumeMountPointA.obj\
        $(SLO)$/FindFirstVolumeMountPointW.obj\
        $(SLO)$/FindNextVolumeMountPointA.obj\
        $(SLO)$/FindNextVolumeMountPointW.obj\
        $(SLO)$/FindVolumeMountPointClose.obj\
        $(SLO)$/GetVolumeNameForVolumeMountPointA.obj\
        $(SLO)$/GetVolumeNameForVolumeMountPointW.obj\
        $(SLO)$/DeleteVolumeMountPointA.obj\
        $(SLO)$/DeleteVolumeMountPointW.obj\
        $(SLO)$/GetVolumePathNameA.obj\
        $(SLO)$/GetVolumePathNameW.obj\
        $(SLO)$/SetVolumeMountPointA.obj\
        $(SLO)$/SetVolumeMountPointW.obj

SHL1TARGET=$(TARGET)
SHL1IMPLIB=$(SHL1TARGET)
SHL1DEF=$(MISC)/$(SHL1TARGET).def
DEF1NAME=$(SHL1TARGET)
DEF1EXPORTFILE=\
    $(SHL1TARGET).dxp\
    unicows.dxp
DEF1DEPN=\
        $(DEF1EXPORTFILE)\
        makefile.mk

#SHL1VERINFO=$(SHL1TARGET).rc
SHL1OBJS=$(SLOFILES)

#No default libraries
STDSHL=

SHL1STDLIBS=\
        unicows.lib\
        kernel32.lib\
        user32.lib\
        advapi32.lib\
        version.lib\
        msvcrt.lib

        
.ENDIF

.INCLUDE : target.mk

