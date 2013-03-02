#ifndef _HyperPage_h_
#define _HyperPage_h_
/* HyperPage.h
 *
 * Copyright (C) 1992-2011,2012 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "Editor.h"
#include "Collection.h"
#include "Graphics.h"

Thing_define (HyperLink, Data) {
	// new data:
	public:
		double x1DC, x2DC, y1DC, y2DC;
};

HyperLink HyperLink_create (const wchar_t *name, double x1, double x2, double y1, double y2);

Thing_define (HyperPage, Editor) {
	// data:
	public:
		GuiDrawingArea drawingArea;
		GuiScrollBar verticalScrollBar;
		Graphics g, ps;
		double d_x, d_y, rightMargin, previousBottomSpacing;
		long d_printingPageNumber;
		Collection links;
		int printing, top, mirror;
		wchar_t *insideHeader, *middleHeader, *outsideHeader;
		wchar_t *insideFooter, *middleFooter, *outsideFooter;
		wchar_t *entryHint; double entryPosition;
		struct { wchar_t *page; int top; } history [20];
		int historyPointer;
		wchar_t *currentPageTitle;
		GuiMenuItem fontSizeButton_10, fontSizeButton_12, fontSizeButton_14, fontSizeButton_18, fontSizeButton_24;
		void *praatApplication, *praatObjects, *praatPicture;
		bool scriptErrorHasBeenNotified;
		structMelderDir rootDirectory;
	// overridden methods:
	protected:
		virtual void v_destroy ();
		virtual bool v_editable () { return false; }
		virtual void v_createMenus ();
		virtual void v_createChildren ();
		virtual void v_dataChanged ();
	// new methods:
	public:
		virtual void v_draw () { }
		virtual long v_getNumberOfPages () { return 0; }
		virtual long v_getCurrentPageNumber () { return 0; }
		virtual int v_goToPage (const wchar_t *title) { (void) title; return 0; }
		virtual void v_goToPage_i (long pageNumber) { (void) pageNumber; }
		virtual void v_defaultHeaders (EditorCommand cmd) { (void) cmd; }
		virtual bool v_hasHistory () { return false; }
		virtual bool v_isOrdered () { return false; }
	// preferences:
		#include "HyperPage_prefs.h"
};

void HyperPage_clear (HyperPage me);

/* "Methods" */
#define HyperPage_ADD_BORDER  1
#define HyperPage_USE_ENTRY_HINT  2

int HyperPage_any (I, const wchar_t *text, enum kGraphics_font font, int size, int style, double minFooterDistance,
	double x, double secondIndent, double topSpacing, double bottomSpacing, unsigned long method);
int HyperPage_pageTitle (I, const wchar_t *title);
int HyperPage_intro (I, const wchar_t *text);
int HyperPage_entry (I, const wchar_t *title);
int HyperPage_paragraph (I, const wchar_t *text);
int HyperPage_listItem (I, const wchar_t *text);
int HyperPage_listItem1 (I, const wchar_t *text);
int HyperPage_listItem2 (I, const wchar_t *text);
int HyperPage_listItem3 (I, const wchar_t *text);
int HyperPage_listTag (I, const wchar_t *text);
int HyperPage_listTag1 (I, const wchar_t *text);
int HyperPage_listTag2 (I, const wchar_t *text);
int HyperPage_listTag3 (I, const wchar_t *text);
int HyperPage_definition (I, const wchar_t *text);
int HyperPage_definition1 (I, const wchar_t *text);
int HyperPage_definition2 (I, const wchar_t *text);
int HyperPage_definition3 (I, const wchar_t *text);
int HyperPage_code (I, const wchar_t *text);
int HyperPage_code1 (I, const wchar_t *text);
int HyperPage_code2 (I, const wchar_t *text);
int HyperPage_code3 (I, const wchar_t *text);
int HyperPage_code4 (I, const wchar_t *text);
int HyperPage_code5 (I, const wchar_t *text);
int HyperPage_prototype (I, const wchar_t *text);
int HyperPage_formula (I, const wchar_t *formula);
int HyperPage_picture (I, double width_inches, double height_inches, void (*draw) (Graphics g));
int HyperPage_script (I, double width_inches, double height_inches, const wchar_t *script);

int HyperPage_goToPage (I, const wchar_t *title);
void HyperPage_goToPage_i (I, long i);

void HyperPage_init (HyperPage me, const wchar_t *title, Data data);

void HyperPage_setEntryHint (I, const wchar_t *entry);
void HyperPage_initSheetOfPaper (HyperPage me);

/* End of file HyperPage.h */
#endif
