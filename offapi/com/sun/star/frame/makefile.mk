#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.28 $
#
#   last change: $Author: kz $ $Date: 2005-03-01 18:18:23 $
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

TARGET=cssframe
PACKAGE=com$/sun$/star$/frame

# --- Settings -----------------------------------------------------
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# ------------------------------------------------------------------------

IDLFILES=\
    BorderWidths.idl\
    CommandGroup.idl\
    Components.idl\
    ContentHandler.idl\
    ContentHandlerFactory.idl\
    Controller.idl\
    Desktop.idl\
    DesktopTask.idl\
    DesktopTasks.idl\
    DispatchDescriptor.idl\
    DispatchHelper.idl\
    DispatchInformation.idl\
    DispatchProvider.idl\
    DispatchRecorder.idl\
    DispatchRecorderSupplier.idl\
    DispatchResultState.idl\
    DispatchResultEvent.idl\
    DispatchStatement.idl\
    DocumentTemplates.idl\
    DoubleInitializationException.idl\
    FeatureStateEvent.idl\
    Frame.idl\
    FrameAction.idl\
    FrameActionEvent.idl\
    FrameControl.idl\
    FrameLoader.idl\
    FrameLoaderFactory.idl\
    FramesContainer.idl\
    FrameSearchFlag.idl\
    IllegalArgumentIOException.idl\
    LayoutManager.idl\
    LayoutManagerEvents.idl\
    MediaTypeDetectionHelper.idl\
    ModuleManager.idl\
    PopupMenuController.idl\
    PopupMenuControllerFactory.idl\
    ProtocolHandler.idl\
    SessionManager.idl\
    Settings.idl\
    SynchronousFrameLoader.idl\
    StatusbarController.idl\
    StatusbarControllerFactory.idl\
    Task.idl\
    TemplateAccess.idl\
    TerminationVetoException.idl\
    ToolbarController.idl\
    TransientDocumentsDocumentContentFactory.idl \
    UnknownModuleException.idl\
    WindowArrange.idl\
    XBorderResizeListener.idl\
    XBrowseHistoryRegistry.idl\
    XComponentLoader.idl\
    XComponentRegistry.idl\
    XConfigManager.idl\
    XController.idl\
    XControllerBorder.idl\
    XDesktop.idl\
    XDesktopTask.idl\
    XDispatch.idl\
    XDispatchHelper.idl\
    XDispatchInformationProvider.idl\
    XDispatchProvider.idl\
    XDispatchProviderInterception.idl\
    XDispatchProviderInterceptor.idl\
    XDispatchRecorder.idl\
    XDispatchRecorderSupplier.idl\
    XDispatchResultListener.idl\
    XDocumentTemplates.idl\
    XExtendedFilterDetection.idl\
    XFilterDetect.idl\
    XFrame.idl\
    XFrameActionListener.idl\
    XFrameLoader.idl\
    XFrameLoaderQuery.idl\
    XFrames.idl\
    XFrameSetModel.idl\
    XFramesSupplier.idl\
    XInplaceLayout.idl\
    XInterceptorInfo.idl\
    XLayoutManager.idl\
    XLayoutManagerEventBroadcaster.idl\
    XLayoutManagerListener.idl\
    XLoadable.idl\
    XLoadEventListener.idl\
    XMenuBarAcceptor.idl\
    XMenuBarMergingAcceptor.idl\
    XModel.idl\
    XModuleManager.idl\
    XNotifyingDispatch.idl\
    XPopupMenuController.idl\
    XRecordableDispatch.idl\
    XSessionManagerClient.idl\
    XSessionManagerListener.idl\
    XStatusListener.idl\
    XStatusbarController.idl\
    XStorable.idl\
    XSubToolbarController.idl\
    XStorable2.idl\
    XSynchronousDispatch.idl\
    XSynchronousFrameLoader.idl\
    XTask.idl\
    XTasksSupplier.idl\
    XTerminateListener.idl\
    XToolbarController.idl\
    XTransientDocumentsDocumentContentFactory.idl\
    XUIControllerRegistration.idl\
    XUrlList.idl\
    XWindowArranger.idl

# ------------------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE :  $(PRJ)$/util$/target.pmk
