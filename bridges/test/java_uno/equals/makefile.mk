#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.5 $
#
#   last change: $Author: rt $ $Date: 2005-09-07 23:00:05 $
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

PRJ = ..$/..$/..
PRJNAME = bridges

TARGET = test_javauno_equals
PACKAGE = test$/java_uno$/equals

ENABLE_EXCEPTIONS = TRUE

.INCLUDE: settings.mk

# Make sure TestBed.class is found under $(CLASSDIR)$/test:
.IF "$(XCLASSPATH)" == ""
XCLASSPATH := $(CLASSDIR)$/test
.ELSE
XCLASSPATH !:= $(XCLASSPATH)$(PATH_SEPERATOR)$(CLASSDIR)$/test
.ENDIF

DLLPRE = # no leading "lib" on .so files
INCPRE += $(MISC)$/$(TARGET)$/inc

SLOFILES = $(SLO)$/testequals.obj

SHL1TARGET = testequals.uno
SHL1OBJS = $(SLOFILES)
SHL1STDLIBS = $(CPPULIB) $(CPPUHELPERLIB) $(SALLIB)
SHL1VERSIONMAP = testequals.map
SHL1IMPLIB = itestequals

JAVAFILES = TestEquals.java
JARFILES = juh.jar jurt.jar ridl.jar

.INCLUDE: target.mk

ALLTAR: $(BIN)$/testequals$(SCRIPTEXT)

.IF "$(GUI)" == "WNT"
GIVE_EXEC_RIGHTS = @echo
.ELSE # GUI, WNT
GIVE_EXEC_RIGHTS = chmod +x
.ENDIF # GUI, WNT

EXEC_CLASSPATH_TMP = $(foreach,i,$(JARFILES) $(SOLARBINDIR)$/$i)
EXEC_CLASSPATH = \
    $(strip $(subst,!,$(PATH_SEPERATOR) $(EXEC_CLASSPATH_TMP:s/ /!/)))

$(MISC)$/$(TARGET).rdb: types.idl
    - rm $@
    - $(MKDIR) $(MISC)$/$(TARGET)
    - $(MKDIR) $(MISC)$/$(TARGET)$/inc
    idlc -I$(SOLARIDLDIR) -O$(MISC)$/$(TARGET) $<
    regmerge $(MISC)$/$(TARGET).rdb /UCR $(MISC)$/$(TARGET)$/types.urd
    cppumaker -BUCR -C -O$(MISC)$/$(TARGET)$/inc $@ -X$(SOLARBINDIR)$/types.rdb
    javamaker -BUCR -nD -O$(CLASSDIR) $@ -X$(SOLARBINDIR)$/types.rdb

$(SLOFILES) $(JAVACLASSFILES): $(MISC)$/$(TARGET).rdb

$(BIN)$/testequals$(SCRIPTEXT): $(BIN)$/testequals_services.rdb
    echo java -classpath \
        ..$/class$/test$(PATH_SEPERATOR)..$/class$(PATH_SEPERATOR)\
..$/class$/java_uno.jar$(PATH_SEPERATOR)$(EXEC_CLASSPATH) \
        test.java_uno.equals.TestEquals $(SOLARBINDIR)$/types.rdb \
        testequals_services.rdb > $@
    $(GIVE_EXEC_RIGHTS) $@

$(BIN)$/testequals_services.rdb:
    - rm $@
    regcomp -register -r $@ -c bridgefac.uno
    regcomp -register -r $@ -c connector.uno
    regcomp -register -r $@ -c remotebridge.uno
