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

PRJ=..$/..$/..$/..

PRJNAME=api

TARGET=csscontainer
PACKAGE=com$/sun$/star$/container

# --- Settings -----------------------------------------------------
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# ------------------------------------------------------------------------

IDLFILES=\
	XIdentifierAccess.idl\
	XIdentifierReplace.idl\
	XIdentifierContainer.idl\
	ContainerEvent.idl\
	ElementExistException.idl\
	EnumerableMap.idl\
	NoSuchElementException.idl\
	XChild.idl\
	XUniqueIDAccess.idl\
	XComponentEnumeration.idl\
	XComponentEnumerationAccess.idl\
	XContainer.idl\
	XContainerApproveBroadcaster.idl\
	XContainerApproveListener.idl\
	XContainerListener.idl\
	XContainerQuery.idl\
	XContentEnumerationAccess.idl\
	XElementAccess.idl\
	XEnumerableMap.idl\
	XEnumeration.idl\
	XEnumerationAccess.idl\
	XHierarchicalName.idl\
	XHierarchicalNameAccess.idl\
	XHierarchicalNameReplace.idl\
	XHierarchicalNameContainer.idl\
	XImplicitIDAccess.idl\
	XImplicitIDContainer.idl\
	XImplicitIDReplace.idl\
	XIndexAccess.idl\
	XIndexContainer.idl\
	XIndexReplace.idl\
	XMap.idl\
	XNameAccess.idl\
	XNameContainer.idl\
	XNamed.idl\
	XNameReplace.idl\
	XSet.idl\
    XStringKeyMap.idl\

# ------------------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE :  $(PRJ)$/util$/target.pmk
