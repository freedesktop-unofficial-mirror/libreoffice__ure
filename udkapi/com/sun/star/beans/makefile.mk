#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.7 $
#
#   last change: $Author: mi $ $Date: 2002-09-02 11:22:46 $
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

PRJ=..$/..$/..$/..

PRJNAME=api

TARGET=cssbeans
PACKAGE=com$/sun$/star$/beans

# --- Settings -----------------------------------------------------
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# ------------------------------------------------------------------------

IDLFILES=\
    IllegalTypeException.idl\
    Introspection.idl\
    IntrospectionException.idl\
    MethodConcept.idl\
    NamedValue.idl\
    NotRemoveableException.idl\
    Property.idl\
    PropertyAttribute.idl\
    PropertySet.idl\
    PropertySetInfo.idl\
    PropertyBag.idl\
    PropertyChangeEvent.idl\
    PropertyConcept.idl\
    PropertyExistException.idl\
    PropertySetInfoChange.idl\
    PropertySetInfoChangeEvent.idl\
    PropertyState.idl\
    PropertyStateChangeEvent.idl\
    PropertyValue.idl\
    PropertyValues.idl\
    PropertyVetoException.idl\
    UnknownPropertyException.idl\
    StringPair.idl\
    XExactName.idl\
    XFastPropertySet.idl\
    XHierarchicalPropertySet.idl\
    XHierarchicalPropertySetInfo.idl\
    XIntrospection.idl\
    XIntrospectionAccess.idl\
    XIntroTest.idl\
    XMaterialHolder.idl\
    XMultiPropertySet.idl\
    XMultiPropertyStates.idl\
    XMultiHierarchicalPropertySet.idl\
    XPropertiesChangeListener.idl\
    XPropertiesChangeNotifier.idl\
    XProperty.idl\
    XPropertyAccess.idl\
    XPropertyChangeListener.idl\
    XPropertyContainer.idl\
    XPropertySet.idl\
    XPropertySetInfo.idl\
    XPropertySetInfoChangeListener.idl\
    XPropertySetInfoChangeNotifier.idl\
    XPropertyState.idl\
    XPropertyStateChangeListener.idl\
    XPropertyWithState.idl\
    XVetoableChangeListener.idl\

# ------------------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE :  $(PRJ)$/util$/target.pmk
