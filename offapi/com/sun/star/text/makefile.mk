#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.33 $
#
#   last change: $Author: hr $ $Date: 2003-03-26 13:57:15 $
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
#   License at http:#www.openoffice.org/license.html.
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

TARGET=csstext
PACKAGE=com$/sun$/star$/text

# --- Settings -----------------------------------------------------
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# ------------------------------------------------------------------------

IDLFILES=\
    AdvancedTextDocument.idl\
    AuthorDisplayFormat.idl\
    AutoTextContainer.idl\
    AutoTextEntry.idl\
    AutoTextGroup.idl\
    BaseFrame.idl\
    BaseFrameProperties.idl\
    BaseIndex.idl\
    BaseIndexMark.idl\
    Bibliography.idl\
    BibliographyDataType.idl\
    BibliographyDataField.idl\
    Bookmark.idl\
    Bookmarks.idl\
    CellProperties.idl\
    CellRange.idl\
    ChainedTextFrame.idl\
    ChapterFormat.idl\
    ChapterNumberingRule.idl\
    CharacterCompressionType.idl\
    ContentIndex.idl\
    ContentIndexMark.idl\
    ControlCharacter.idl\
    DateDisplayFormat.idl\
    DefaultNumberingProvider.idl\
    Defaults.idl\
    DependentTextField.idl\
    DocumentIndex.idl\
    DocumentIndexes.idl\
    DocumentIndexFormat.idl\
    DocumentIndexLevelFormat.idl\
    DocumentIndexMark.idl\
    DocumentIndexMarkAsian.idl\
    DocumentIndexParagraphStyles.idl\
    DocumentSettings.idl\
    DocumentStatistic.idl\
    Endnote.idl\
        EndnoteSettings.idl\
    FilenameDisplayFormat.idl\
    FontEmphasis.idl\
    FontRelief.idl\
    Footnote.idl\
     FootnoteNumbering.idl\
     Footnotes.idl\
     FootnoteSettings.idl\
     GlobalSettings.idl\
     GraphicCrop.idl\
     HoriOrientation.idl\
     HoriOrientationFormat.idl\
     HorizontalAdjust.idl\
     HypertextDocument.idl\
     IllustrationIndex.idl\
     InvalidTextContentException.idl\
     LineNumberingProperties.idl\
     LineNumberingSettings.idl\
        MailMerge.idl\
        MailMergeEvent.idl\
        MailMergeType.idl\
     NotePrintMode.idl\
     NumberingLevel.idl\
     NumberingRules.idl\
     NumberingStyle.idl\
     ObjectIndex.idl\
     PageFootnoteInfo.idl\
     PageNumberType.idl\
     PagePrintSettings.idl\
     Paragraph.idl\
     ParagraphEnumeration.idl\
     ParagraphVertAlign.idl\
     PlaceholderType.idl\
     PrintPreviewSettings.idl\
     PrintSettings.idl\
     RedlinePortion.idl\
     ReferenceFieldPart.idl\
     ReferenceFieldSource.idl\
     ReferenceMark.idl\
     ReferenceMarks.idl\
     RelOrientation.idl\
     RubyAdjust.idl\
     SectionFileLink.idl\
     SetVariableType.idl\
     Shape.idl\
     SizeType.idl\
        TableColumns.idl\
     TableColumnSeparator.idl\
     TableIndex.idl\
        TableRows.idl\
     TemplateDisplayFormat.idl\
     Text.idl\
     TextColumn.idl\
    TextColumns.idl\
     TextColumnSequence.idl\
     TextContent.idl\
     TextContentAnchorType.idl\
     TextContentCollection.idl\
     TextCursor.idl\
     TextDocument.idl\
     TextDocumentView.idl\
     TextEmbeddedObject.idl\
     TextEmbeddedObjects.idl\
     TextField.idl\
     TextFieldEnumeration.idl\
     TextFieldMaster.idl\
     TextFieldMasters.idl\
     TextFields.idl\
     TextFrame.idl\
     TextFrames.idl\
     TextGraphicObject.idl\
     TextGraphicObjects.idl\
     TextGridMode.idl\
      TextLayoutCursor.idl\
      TextPageStyle.idl\
      TextPortion.idl\
      TextPortionEnumeration.idl\
      TextRange.idl\
      TextRanges.idl\
      TextSection.idl\
      TextSections.idl\
      TextSortable.idl\
      TextSortDescriptor.idl\
      TextTable.idl\
      TextTableCursor.idl\
      TextTableRow.idl\
      TextTables.idl\
      TextViewCursor.idl\
      TimeDisplayFormat.idl\
      UserDataPart.idl\
      UserDefinedIndex.idl\
      UserFieldFormat.idl\
      UserIndex.idl\
      UserIndexMark.idl\
      VertOrientation.idl\
      VertOrientationFormat.idl\
      ViewSettings.idl\
      WrapTextMode.idl\
      WritingMode.idl\
      WritingMode2.idl\
      XAutoTextContainer.idl\
      XAutoTextEntry.idl\
      XAutoTextGroup.idl\
      XBookmarkInsertTool.idl\
      XBookmarksSupplier.idl\
      XChapterNumberingSupplier.idl\
      XDefaultNumberingProvider.idl\
      XDependentTextField.idl\
      XDocumentIndex.idl\
      XDocumentIndexesSupplier.idl\
      XDocumentIndexMark.idl\
      XEndnotesSettingsSupplier.idl\
      XEndnotesSupplier.idl\
      XFootnote.idl\
      XFootnotesSettingsSupplier.idl\
      XFootnotesSupplier.idl\
      XHeaderFooter.idl\
      XHeaderFooterPageStyle.idl\
      XLineNumberingSupplier.idl\
      XLineNumberingProperties.idl\
        XMailMergeBroadcaster.idl\
        XMailMergeListener.idl\
      XModule.idl\
      XNumberingFormatter.idl\
      XNumberingRulesSupplier.idl\
      XNumberingTypeInfo.idl\
      XPageCursor.idl\
      XPagePrintable.idl\
      XParagraphCursor.idl\
      XReferenceMarksSupplier.idl\
      XRelativeTextContentInsert.idl\
      XRelativeTextContentRemove.idl\
      XRubySelection.idl\
      XSentenceCursor.idl\
      XSimpleText.idl\
      XText.idl\
      XTextColumns.idl\
      XTextContent.idl\
      XTextCursor.idl\
      XTextDocument.idl\
      XTextEmbeddedObject.idl\
      XTextEmbeddedObjectsSupplier.idl\
      XTextField.idl\
      XTextFieldsSupplier.idl\
      XTextFrame.idl\
      XTextFramesSupplier.idl\
      XTextGraphicObjectsSupplier.idl\
      XTextRange.idl\
      XTextRangeCompare.idl\
      XTextRangeMover.idl\
      XTextSection.idl\
      XTextSectionsSupplier.idl\
      XTextShapesSupplier.idl\
      XTextTable.idl\
      XTextTableCursor.idl\
      XTextTablesSupplier.idl\
      XTextViewCursor.idl\
      XTextViewCursorSupplier.idl\
      XWordCursor.idl

# ------------------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE :  $(PRJ)$/util$/target.pmk
