#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.40 $
#
#   last change: $Author: rt $ $Date: 2005-09-08 09:28:11 $
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
PRJ=..

PRJNAME=cppuhelper
TARGET=cppuhelper

NO_BSYMBOLIC=TRUE
ENABLE_EXCEPTIONS=TRUE
USE_DEFFILE=TRUE

.IF "$(OS)" != "WNT"
UNIXVERSIONNAMES=UDK
.ENDIF # WNT

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# --- Files --------------------------------------------------------

#UNOUCRRDB=$(OUT)$/bin$/udkapi.rdb
UNOUCRRDB=$(SOLARBINDIR)$/udkapi.rdb
UNOUCRDEP=$(UNOUCRRDB)
UNOUCROUT=$(OUT)$/inc
INCPRE+=$(OUT)$/inc $(OUT)$/inc$/private

CPPUMAKERFLAGS=

UNOTYPES= \
        com.sun.star.registry.XSimpleRegistry		\
        com.sun.star.registry.XImplementationRegistration \
        com.sun.star.container.XSet			\
        com.sun.star.container.XNameAccess		\
        com.sun.star.uno.XWeak 				\
        com.sun.star.uno.XAggregation 			\
        com.sun.star.uno.XComponentContext		\
        com.sun.star.uno.XUnloadingPreference    	\
        com.sun.star.uno.DeploymentException    	\
        com.sun.star.uno.RuntimeException \
        com.sun.star.lang.WrappedTargetRuntimeException \
        com.sun.star.lang.DisposedException 		\
        com.sun.star.lang.XMultiServiceFactory 		\
        com.sun.star.lang.XSingleServiceFactory 	\
        com.sun.star.lang.XMultiComponentFactory 	\
        com.sun.star.lang.XSingleComponentFactory 	\
        com.sun.star.lang.XServiceInfo 			\
        com.sun.star.lang.XInitialization 		\
        com.sun.star.lang.XEventListener 		\
        com.sun.star.reflection.XIdlReflection 		\
        com.sun.star.reflection.XIdlClass 		\
        com.sun.star.reflection.XIdlClassProvider	\
        com.sun.star.reflection.XIndirectTypeDescription	\
        com.sun.star.reflection.XMethodParameter		\
        com.sun.star.reflection.XInterfaceMemberTypeDescription	\
        com.sun.star.reflection.XTypeDescription		\
        com.sun.star.reflection.XEnumTypeDescription		\
        com.sun.star.reflection.XArrayTypeDescription		\
        com.sun.star.reflection.XInterfaceAttributeTypeDescription \
        com.sun.star.reflection.XInterfaceMethodTypeDescription	\
        com.sun.star.reflection.XInterfaceTypeDescription2	\
        com.sun.star.reflection.XCompoundTypeDescription	\
        com.sun.star.reflection.XStructTypeDescription \
        com.sun.star.reflection.XUnionTypeDescription	\
        com.sun.star.beans.XPropertySet 		\
        com.sun.star.beans.XMultiPropertySet 		\
        com.sun.star.beans.PropertyValue 		\
        com.sun.star.beans.XFastPropertySet 		\
        com.sun.star.beans.PropertyAttribute 		\
        com.sun.star.container.XNameContainer \
        com.sun.star.container.XHierarchicalNameAccess	\
        com.sun.star.registry.XRegistryKey		\
        com.sun.star.loader.XImplementationLoader	\
        com.sun.star.lang.XTypeProvider			\
        com.sun.star.lang.XComponent			\
        com.sun.star.uno.XCurrentContext		\
        com.sun.star.security.XAccessController		\
        com.sun.star.security.RuntimePermission		\
        com.sun.star.io.FilePermission			\
        com.sun.star.io.IOException			\
        com.sun.star.connection.SocketPermission	\
        com.sun.star.util.XMacroExpander

.IF "$(debug)" != ""
# msvc++: no inlining for debugging
.IF "$(COM)" == "MSC"
CFLAGS += -Ob0
.ENDIF
.ENDIF

SLOFILES= \
        $(SLO)$/typeprovider.obj 	\
        $(SLO)$/exc_thrower.obj 	\
        $(SLO)$/servicefactory.obj 	\
        $(SLO)$/bootstrap.obj 		\
        $(SLO)$/implbase.obj 		\
        $(SLO)$/implbase_ex.obj 	\
        $(SLO)$/propshlp.obj 		\
        $(SLO)$/weak.obj		\
        $(SLO)$/interfacecontainer.obj	\
        $(SLO)$/stdidlclass.obj 	\
        $(SLO)$/factory.obj		\
        $(SLO)$/component_context.obj	\
        $(SLO)$/component.obj		\
        $(SLO)$/shlib.obj		\
        $(SLO)$/tdmgr.obj		\
        $(SLO)$/implementationentry.obj	\
        $(SLO)$/access_control.obj	\
        $(SLO)$/macro_expander.obj \
        $(SLO)$/unourl.obj

.IF "$(GUI)" == "WNT"
SHL1TARGET=$(TARGET)$(UDK_MAJOR)$(COMID)
.ELSE
SHL1TARGET=uno_$(TARGET)$(COMID)
.ENDIF

SHL1STDLIBS= \
        $(SALLIB)		\
        $(CPPULIB)

SHL1DEPN=
SHL1IMPLIB=i$(TARGET)
SHL1OBJS = $(SLOFILES)

SHL1DEF=$(MISC)$/$(SHL1TARGET).def

DEF1NAME=$(SHL1TARGET)

.IF "$(COMNAME)"=="msci"
SHL1VERSIONMAP=msvc_win32_intel.map
.ELIF "$(COMNAME)"=="sunpro5"
SHL1VERSIONMAP=cc5_solaris_sparc.map
.ELIF "$(OS)$(CPU)$(COMNAME)"=="LINUXIgcc3"
SHL1VERSIONMAP=gcc3_linux_intel.map
.ELIF "$(OS)$(CPU)$(COMNAME)"=="FREEBSDIgcc3"
SHL1VERSIONMAP=gcc3_linux_intel.map
.ELIF "$(OS)$(CPU)$(COMNAME)"=="LINUXSgcc3"
SHL1VERSIONMAP=gcc3_linux_intel.map
.ENDIF

# --- Targets ------------------------------------------------------

.IF "$(diag)"!=""
CFLAGS += -DDIAG=$(diag)
.ENDIF

.INCLUDE :	target.mk
