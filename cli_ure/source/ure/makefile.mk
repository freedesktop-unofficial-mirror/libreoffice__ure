#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.3 $
#
#   last change: $Author: dbo $ $Date: 2003-05-08 12:41:02 $
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

.INCLUDE : $(PRJ)$/util$/makefile.pmk
.IF "$(BUILD_FOR_CLI)" != ""

# for dummy
TARGET = cli_ure

.INCLUDE : settings.mk

.INCLUDE : $(PRJ)$/util$/target.pmk
.INCLUDE : target.mk

ALLTAR : \
    $(OUT)$/bin$/cli_ure.dll

CSFILES = \
    uno$/QueryInterface.cs						\
    uno$/Runtime.cs							\
    uno$/ServiceImplementationAttribute.cs				\
    uno$/SingletonAttribute.cs					\
    uno$/util$/DisposeGuard.cs					\
    uno$/util$/WeakAdapter.cs					\
    uno$/util$/WeakBase.cs						\
    uno$/util$/WeakComponentBase.cs					\
    uno$/util$/ServiceBase.cs					\
    uno$/util$/ServiceFactory.cs					\
    uno$/util$/DeployedTypeResolver.cs

$(OUT)$/bin$/cli_ure.dll : $(CSFILES) $(OUT)$/bin$/cli_types.dll
    +csc $(CSCFLAGS) \
        -target:library \
        -out:$@ \
        -reference:$(OUT)$/bin$/cli_types.dll \
        -reference:System.dll \
        $(CSFILES) 

.ENDIF
