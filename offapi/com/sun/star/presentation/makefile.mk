#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.11 $
#
#   last change: $Author: rt $ $Date: 2004-11-26 17:46:00 $
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

TARGET=csspresentation
PACKAGE=com$/sun$/star$/presentation

# --- Settings -----------------------------------------------------
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# ------------------------------------------------------------------------

IDLFILES=\
    HeaderShape.idl\
    FooterShape.idl\
    DateTimeShape.idl\
    SlideNumberShape.idl\
    AnimationEffect.idl\
    AnimationSpeed.idl\
    ChartShape.idl\
    ClickAction.idl\
    CustomPresentation.idl\
    CustomPresentationAccess.idl\
    DocumentSettings.idl\
    DrawPage.idl\
    FadeEffect.idl\
    GraphicObjectShape.idl\
    HandoutShape.idl\
    HandoutView.idl\
    NotesShape.idl\
    NotesView.idl\
    OLE2Shape.idl\
    OutlinerShape.idl\
    OutlineView.idl\
    PageShape.idl\
    Presentation.idl\
    PresentationDocument.idl\
    PresentationRange.idl\
    PresentationView.idl\
    PreviewView.idl\
    Shape.idl\
    SlidesView.idl\
    SubtitleShape.idl\
    TitleTextShape.idl\
    XCustomPresentationSupplier.idl\
    XHandoutMasterSupplier.idl\
    XPresentation.idl\
    XPresentationPage.idl\
    XPresentationSupplier.idl\
    EffectNodeType.idl\
    EffectPresetClass.idl\
    ParagraphTarget.idl\
    ShapeAnimationSubType.idl\
    TextAnimationType.idl\
    EffectCommands.idl

# ------------------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE :  $(PRJ)$/util$/target.pmk
