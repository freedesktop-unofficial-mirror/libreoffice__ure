#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.4 $
#
#   last change: $Author: vg $ $Date: 2003-12-17 17:43:38 $
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

PRJ		= ..
PRJNAME	= ridljar
TARGET	= ridl

# --- Settings -----------------------------------------------------
USE_UDK_EXTENDED_MANIFESTFILE=TRUE
USE_EXTENDED_MANIFESTFILE=TRUE

.INCLUDE :  settings.mk

MAXLINELENGTH = 100000

JARCLASSDIRS	= com
JARTARGET		= $(TARGET).jar
JARCOMPRESS 	= TRUE

# Special work necessary for building java reference with javadoc.
# The source of puplic APIs must be delivered and used later in the
# odk module.
ZIP1TARGET=$(TARGET)_src
ZIP1FLAGS=-u -r
ZIP1DIR=$(PRJ)
ZIP1LIST=com -x "*makefile.mk"

.IF "$(SOLAR_JAVA)"!=""
ZIP2TARGET=$(TARGET)2_src
ZIP2FLAGS=-u -r
ZIP2DIR=$(MISC)$/java
ZIP2LIST=com
.ENDIF

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
