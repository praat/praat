#ifndef _HyperPage_h_
#define _HyperPage_h_
/* HyperPage.h
 *
 * Copyright (C) 1992-2024 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptEditor.h"
#include "Collection.h"
#include "Graphics.h"
Thing_declare (PraatApplication);
Thing_declare (PraatObjects);
Thing_declare (PraatPicture);

Thing_define (HyperLink, Daata) {
	double x1DC, x2DC, y1DC, y2DC;
};

autoHyperLink HyperLink_create (conststring32 name, double x1, double x2, double y1, double y2);

Thing_define (HyperPage, Editor) {
	GuiDrawingArea drawingArea;
	GuiScrollBar verticalScrollBar;
	bool d_hasExtraRowOfTools;
	autoGraphics graphics;
	Graphics ps;
	double d_x, d_y, rightMargin, previousBottomSpacing;
	integer d_printingPageNumber;
	CollectionOf<structHyperLink> links;
	bool printing, mirror;
	double top;
	conststring32 insideHeader, middleHeader, outsideHeader;
	conststring32 insideFooter, middleFooter, outsideFooter;
	autostring32 entryHint; double entryPosition;
	struct { autostring32 page; double top; } history [20];
	int historyPointer;
	autostring32 optionalCurrentPageTitle;
	GuiMenuItem fontSizeButton_10, fontSizeButton_12, fontSizeButton_14, fontSizeButton_18, fontSizeButton_24;
	PraatApplication praatApplication;
	PraatObjects praatObjects;
	PraatPicture praatPicture;
	structMelderFolder rootDirectory;

	void v9_destroy () noexcept
		override;
	bool v_hasEditMenu ()
		override { return false; }
	void v_createMenus ()
		override;
	void v_createChildren ()
		override;
	void v1_dataChanged (Editor sender)
		override;

	virtual void v_draw () { }
	virtual integer v_getNumberOfPages () { return 0; }
	virtual integer v_getCurrentPageNumber () { return 0; }
	virtual int v_goToPage (conststring32 /* title */) { return 0; }
	virtual void v_goToPage_number (integer /* goToPageNumber */) { }
	virtual void v_defaultHeaders (EditorCommand /* cmd */) { }
	virtual bool v_hasHistory () { return false; }
	virtual bool v_isOrdered () { return false; }

	#include "HyperPage_prefs.h"
};

void HyperPage_clear (HyperPage me);

/* "Methods" */
#define HyperPage_ADD_BORDER  1
#define HyperPage_USE_ENTRY_HINT  2
#define HyperPage_EXECUTABLE  4

void HyperPage_any (HyperPage me, conststring32 text, kGraphics_font font, double size, int style, double minFooterDistance,
	double x, double secondIndent, double topSpacing, double bottomSpacing, uint32 method);
void HyperPage_pageTitle (HyperPage me, conststring32 title);
void HyperPage_intro (HyperPage me, conststring32 text);
void HyperPage_entry (HyperPage me, conststring32 title);
void HyperPage_subheader (HyperPage me, conststring32 title);
void HyperPage_paragraph (HyperPage me, conststring32 text);
void HyperPage_listItem (HyperPage me, conststring32 text);
void HyperPage_listItem1 (HyperPage me, conststring32 text);
void HyperPage_listItem2 (HyperPage me, conststring32 text);
void HyperPage_listItem3 (HyperPage me, conststring32 text);
void HyperPage_listTag (HyperPage me, conststring32 text);
void HyperPage_listTag1 (HyperPage me, conststring32 text);
void HyperPage_listTag2 (HyperPage me, conststring32 text);
void HyperPage_listTag3 (HyperPage me, conststring32 text);
void HyperPage_definition (HyperPage me, conststring32 text);
void HyperPage_definition1 (HyperPage me, conststring32 text);
void HyperPage_definition2 (HyperPage me, conststring32 text);
void HyperPage_definition3 (HyperPage me, conststring32 text);
void HyperPage_code0 (HyperPage me, conststring32 text);
void HyperPage_code (HyperPage me, conststring32 text);
void HyperPage_code1 (HyperPage me, conststring32 text);
void HyperPage_code2 (HyperPage me, conststring32 text);
void HyperPage_code3 (HyperPage me, conststring32 text);
void HyperPage_code4 (HyperPage me, conststring32 text);
void HyperPage_code5 (HyperPage me, conststring32 text);
void HyperPage_caption (HyperPage me, conststring32 text);
void HyperPage_quote (HyperPage me, conststring32 text);
void HyperPage_quote1 (HyperPage me, conststring32 text);
void HyperPage_quote2 (HyperPage me, conststring32 text);
void HyperPage_quote3 (HyperPage me, conststring32 text);
void HyperPage_prototype (HyperPage me, conststring32 text);
void HyperPage_formula (HyperPage me, conststring32 formula);
void HyperPage_picture (HyperPage me, double width_inches, double height_inches, void (*draw) (Graphics g));
void HyperPage_script (HyperPage me, double width_inches, double height_inches, conststring32 script,
		Graphics cacheGraphics, conststring32 cacheInfo);

int HyperPage_goToPage (HyperPage me, conststring32 title);
void HyperPage_goToPage_number (HyperPage me, integer goToPageNumber);

void HyperPage_init (HyperPage me, conststring32 title, Daata data);
void HyperPage_init1 (HyperPage me, conststring32 title, Daata data, bool backquoteIsVerbatim);
void HyperPage_init2 (HyperPage me, conststring32 title, Daata data);

void HyperPage_setEntryHint (HyperPage me, conststring32 entry);
void HyperPage_initSheetOfPaper (HyperPage me);

/* End of file HyperPage.h */
#endif
