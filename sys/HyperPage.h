#ifndef _HyperPage_h_
#define _HyperPage_h_
/* HyperPage.h
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

/*
 * pb 2011/07/15
 */

#include "Editor.h"
#include "Collection.h"
#include "Graphics.h"

Thing_declare1cpp (HyperLink);
struct structHyperLink : public structData {
	double x1DC, x2DC, y1DC, y2DC;
};
#define HyperLink__methods(klas) Data__methods(klas)
Thing_declare2cpp (HyperLink, Data);

HyperLink HyperLink_create (const wchar *name, double x1, double x2, double y1, double y2);

Thing_declare1cpp (HyperPage);
struct structHyperPage : public structEditor {
	// data:
		GuiObject drawingArea, verticalScrollBar;
		Graphics g, ps;
		double x, y, rightMargin, previousBottomSpacing;
		long pageNumber;
		Collection links;
		int printing, top, mirror;
		wchar *insideHeader, *middleHeader, *outsideHeader;
		wchar *insideFooter, *middleFooter, *outsideFooter;
		enum kGraphics_font font;
		int fontSize;
		wchar *entryHint; double entryPosition;
		struct { wchar *page; int top; } history [20];
		int historyPointer;
		wchar *currentPageTitle;
		GuiObject fontSizeButton_10, fontSizeButton_12, fontSizeButton_14, fontSizeButton_18, fontSizeButton_24;
		GuiObject holder;
		void *praatApplication, *praatObjects, *praatPicture;
		bool scriptErrorHasBeenNotified;
		structMelderDir rootDirectory;
	// overridden methods:
		void v_destroy ();
		bool v_editable () { return false; }
		void v_createMenus ();
		void v_createChildren ();
		void v_dataChanged ();
	// new methods:
};
#define HyperPage__methods(Klas) \
	void (*draw) (Klas me); \
	long (*getNumberOfPages) (Klas me); \
	long (*getCurrentPageNumber) (Klas me); \
	int (*goToPage) (Klas me, const wchar *title); \
	int (*goToPage_i) (Klas me, long ipage); \
	void (*defaultHeaders) (EditorCommand cmd); \
	int hasHistory, isOrdered;
Thing_declare2cpp (HyperPage, Editor);

void HyperPage_clear (HyperPage me);

/* "Methods" */
#define HyperPage_ADD_BORDER  1
#define HyperPage_USE_ENTRY_HINT  2

int HyperPage_any (I, const wchar *text, enum kGraphics_font font, int size, int style, double minFooterDistance,
	double x, double secondIndent, double topSpacing, double bottomSpacing, unsigned long method);
int HyperPage_pageTitle (I, const wchar *title);
int HyperPage_intro (I, const wchar *text);
int HyperPage_entry (I, const wchar *title);
int HyperPage_paragraph (I, const wchar *text);
int HyperPage_listItem (I, const wchar *text);
int HyperPage_listItem1 (I, const wchar *text);
int HyperPage_listItem2 (I, const wchar *text);
int HyperPage_listItem3 (I, const wchar *text);
int HyperPage_listTag (I, const wchar *text);
int HyperPage_listTag1 (I, const wchar *text);
int HyperPage_listTag2 (I, const wchar *text);
int HyperPage_listTag3 (I, const wchar *text);
int HyperPage_definition (I, const wchar *text);
int HyperPage_definition1 (I, const wchar *text);
int HyperPage_definition2 (I, const wchar *text);
int HyperPage_definition3 (I, const wchar *text);
int HyperPage_code (I, const wchar *text);
int HyperPage_code1 (I, const wchar *text);
int HyperPage_code2 (I, const wchar *text);
int HyperPage_code3 (I, const wchar *text);
int HyperPage_code4 (I, const wchar *text);
int HyperPage_code5 (I, const wchar *text);
int HyperPage_prototype (I, const wchar *text);
int HyperPage_formula (I, const wchar *formula);
int HyperPage_picture (I, double width_inches, double height_inches, void (*draw) (Graphics g));
int HyperPage_script (I, double width_inches, double height_inches, const wchar *script);

int HyperPage_goToPage (I, const wchar *title);
int HyperPage_goToPage_i (I, long i);

void HyperPage_init (HyperPage me, GuiObject parent, const wchar *title, Data data);

void HyperPage_prefs (void);
void HyperPage_setEntryHint (I, const wchar *entry);
void HyperPage_initSheetOfPaper (HyperPage me);

/* End of file HyperPage.h */
#endif
