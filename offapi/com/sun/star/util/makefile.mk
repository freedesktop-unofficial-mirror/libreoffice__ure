#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.20 $
#
#   last change: $Author: kz $ $Date: 2004-01-19 18:27:03 $
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

TARGET=cssutil
PACKAGE=com$/sun$/star$/util

# --- Settings -----------------------------------------------------
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# ------------------------------------------------------------------------

IDLFILES=\
    MeasureUnit.idl\
    AliasProgrammaticPair.idl \
    CellProtection.idl\
    ChangesEvent.idl\
    ChangesSet.idl \
    CloseVetoException.idl\
    color.idl\
    Date.idl\
    DateTime.idl\
    DateTimeRange.idl\
    DiskFullException.idl\
    ElementChange.idl \
    FileIOException.idl\
    Language.idl\
    NumberFormat.idl\
    NumberFormatProperties.idl\
    NumberFormats.idl\
    NumberFormatSettings.idl\
    NumberFormatsSupplier.idl\
    NumberFormatter.idl\
    ModeChangeEvent.idl\
    ReplaceDescriptor.idl\
    SearchDescriptor.idl\
        Sortable.idl\
    SortDescriptor.idl\
        SortDescriptor2.idl\
    SortField.idl\
    SortFieldType.idl\
    TextSearch.idl\
    Time.idl\
    URL.idl\
    URLTransformer.idl\
    XArchiver.idl\
    XCancellable.idl\
    XCancelManager.idl\
    XChainable.idl\
    XChangesBatch.idl \
    XChangesListener.idl \
    XChangesNotifier.idl \
    XChangesSet.idl	\
    XCloneable.idl\
    XCloseable.idl\
    XCloseBroadcaster.idl\
    XCloseListener.idl\
    XFlushable.idl\
    XFlushListener.idl\
    XImportable.idl\
    XIndent.idl\
    XLinkUpdate.idl\
    XLocalizedAliases.idl\
    XMergeable.idl\
    XModeChangeBroadcaster.idl\
    XModeChangeListener.idl\
    XModeChangeApproveListener.idl\
    XModeSelector.idl\
    XModifiable.idl\
    XModifyBroadcaster.idl\
    XModifyListener.idl\
    XNumberFormatPreviewer.idl\
    XNumberFormats.idl\
    XNumberFormatsSupplier.idl\
    XNumberFormatter.idl\
    XNumberFormatTypes.idl\
    XPropertyReplace.idl\
    XProtectable.idl\
    XRefreshable.idl\
    XRefreshListener.idl\
    XReplaceable.idl\
    XReplaceDescriptor.idl\
       XSearchable.idl\
       XSearchDescriptor.idl\
    XSimpleErrorHandler.idl\
    XSortable.idl\
    XStringAbbreviation.idl\
    XStringWidth.idl\
    XStringMapping.idl\
    XStringEscape.idl\
    XTextSearch.idl\
    XUpdatable.idl\
    XURLTransformer.idl\
    XUniqueIDFactory.idl\
    VetoException.idl\
    DataEditorEventType.idl\
    DataEditorEvent.idl\
    XDataEditorListener.idl\
    XDataEditor.idl\
    MalformedNumberFormatException.idl\
    NotNumericException.idl\
    XAtomServer.idl\
    AtomClassRequest.idl\
    AtomDescription.idl\
    XStringSubstitution.idl\
    PathSettings.idl\
    PathSubstitution.idl\
    XTimeStamped.idl

# ------------------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE :  $(PRJ)$/util$/target.pmk
