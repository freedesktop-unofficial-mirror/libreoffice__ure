#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.18 $
#
#   last change: $Author: vg $ $Date: 2003-05-28 13:31:51 $
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
#   See the License for the specific p	XTypeDetection.idl\
rovisions governing your rights and
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
    Components.idl\
    ContentHandler.idl\
    ContentHandlerFactory.idl\
    Controller.idl\
    Desktop.idl\
    DesktopTask.idl\
    DesktopTasks.idl\
    DispatchDescriptor.idl\
    DispatchHelper.idl\
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
    MediaTypeDetectionHelper.idl\
    ProtocolHandler.idl\
    Settings.idl\
    SynchronousFrameLoader.idl\
    Task.idl\
    TemplateAccess.idl\
    TerminationVetoException.idl\
    WindowArrange.idl\
    XBrowseHistoryRegistry.idl\
    XComponentLoader.idl\
    XComponentRegistry.idl\
    XConfigManager.idl\
    XController.idl\
    XDesktop.idl\
    XDesktopTask.idl\
    XDispatch.idl\
    XDispatchHelper.idl\
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
    XInterceptorInfo.idl\
    XLoadable.idl\
    XLoadEventListener.idl\
    XModel.idl\
    XNotifyingDispatch.idl\
    XRecordableDispatch.idl\
    XStatusListener.idl\
    XStorable.idl\
    XSynchronousDispatch.idl\
    XSynchronousFrameLoader.idl\
    XTask.idl\
    XTasksSupplier.idl\
    XTerminateListener.idl\
    XUrlList.idl\
    XWindowArranger.idl\

# ------------------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE :  $(PRJ)$/util$/target.pmk
