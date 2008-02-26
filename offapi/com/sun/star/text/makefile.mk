#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.44 $
#
#   last change: $Author: obo $ $Date: 2008-02-26 09:54:20 $
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

PRJ=..$/..$/..$/..

PRJNAME=offapi

TARGET=csstext
PACKAGE=com$/sun$/star$/text

# --- Settings -----------------------------------------------------
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# ------------------------------------------------------------------------

IDLFILES=\
    AccessibleEndnoteView.idl\
    AccessibleFootnoteView.idl\
    AccessibleHeaderFooterView.idl\
    AccessiblePageView.idl\
    AccessibleParagraphView.idl\
    AccessibleTextDocumentPageView.idl\
    AccessibleTextDocumentView.idl\
    AccessibleTextEmbeddedObject.idl\
    AccessibleTextFrameView.idl\
    AccessibleTextGraphicObject.idl\
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
    Cell.idl\
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
    GenericTextDocument.idl\
    GlobalDocument.idl\
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
        PositionLayoutDir.idl\
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
    TextMarkupType.idl \
      TextPageStyle.idl\
      TextPortion.idl\
      TextPortionEnumeration.idl\
      TextRange.idl\
      TextRanges.idl\
      TextSection.idl\
      TextSections.idl\
      TextSortable.idl\
      TextSortDescriptor.idl\
        TextSortDescriptor2.idl\
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
    WebDocument.idl\
        WrapInfluenceOnPosition.idl\
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
    XTextMarkup.idl\
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
    XWordCursor.idl \
    XParagraphAppend.idl \
    XTextPortionAppend.idl \
    XTextContentAppend.idl \
    XTextConvert.idl \
    XTextAppendAndConvert.idl \
    XTextAppend.idl \
        XFlatParagraph.idl \
        XFlatParagraphIterator.idl \
        XFlatParagraphIteratorProvider.idl


# ------------------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE :  $(PRJ)$/util$/target.pmk
