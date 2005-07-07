#ifndef _HyperPage_h_
#define _HyperPage_h_
/* HyperPage.h
 *
 * Copyright (C) 1992-2005 Paul Boersma
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
 * pb 2002/03/07 GPL
 * pb 2005/05/06 HyperPage_script
 */

#ifndef _Editor_h_
	#include "Editor.h"
#endif
#ifndef _Collection_h_
	#include "Collection.h"
#endif
#ifndef _Graphics_h_
	#include "Graphics.h"
#endif

#define HyperLink_members Data_members \
	double x1, x2, y1, y2;
#define HyperLink_methods Data_methods
class_create (HyperLink, Data)

HyperLink HyperLink_create (const char *name, double x1, double x2, double y1, double y2);

#define HyperPage_members Editor_members \
	Widget drawingArea, horizontalScrollBar, pageLabel, verticalScrollBar; \
	Graphics g, ps; \
	double x, y, rightMargin, previousBottomSpacing; \
	long pageNumber; \
	Collection links; \
	int printing, top, mirror; \
	char *insideHeader, *middleHeader, *outsideHeader; \
	char *insideFooter, *middleFooter, *outsideFooter; \
	int font, fontSize; \
	char *entryHint; double entryPosition; \
	struct { char *page; int top; } history [20]; \
	int historyPointer; \
	char *currentPageTitle; \
	Widget fontSizeButton_10, fontSizeButton_12, fontSizeButton_14, fontSizeButton_18, fontSizeButton_24;
#define HyperPage_methods Editor_methods \
	void (*draw) (I); \
	long (*getNumberOfPages) (I); \
	long (*getCurrentPageNumber) (I); \
	int (*goToPage) (I, const char *title); \
	int (*goToPage_i) (I, long ipage); \
	void (*defaultHeaders) (EditorCommand cmd); \
	int hasHistory, isOrdered, canIndex;
class_create (HyperPage, Editor)

void HyperPage_clear (HyperPage me);

/* "Methods" */
#define HyperPage_ADD_BORDER  1
#define HyperPage_USE_ENTRY_HINT  2

int HyperPage_any (I, const char *text, int font, int size, int style, double minFooterDistance,
	double x, double secondIndent, double topSpacing, double bottomSpacing, unsigned long method);
int HyperPage_pageTitle (I, const char *title);
int HyperPage_intro (I, const char *text);
int HyperPage_entry (I, const char *title);
int HyperPage_paragraph (I, const char *text);
int HyperPage_listItem (I, const char *text);
int HyperPage_listItem1 (I, const char *text);
int HyperPage_listItem2 (I, const char *text);
int HyperPage_listItem3 (I, const char *text);
int HyperPage_listTag (I, const char *text);
int HyperPage_listTag1 (I, const char *text);
int HyperPage_listTag2 (I, const char *text);
int HyperPage_listTag3 (I, const char *text);
int HyperPage_definition (I, const char *text);
int HyperPage_definition1 (I, const char *text);
int HyperPage_definition2 (I, const char *text);
int HyperPage_definition3 (I, const char *text);
int HyperPage_code (I, const char *text);
int HyperPage_code1 (I, const char *text);
int HyperPage_code2 (I, const char *text);
int HyperPage_code3 (I, const char *text);
int HyperPage_code4 (I, const char *text);
int HyperPage_code5 (I, const char *text);
int HyperPage_prototype (I, const char *text);
int HyperPage_formula (I, const char *formula);
int HyperPage_picture (I, double width_inches, double height_inches, void (*draw) (Graphics g));
int HyperPage_script (I, double width_inches, double height_inches, const char *script);

int HyperPage_goToPage (I, const char *title);
int HyperPage_goToPage_i (I, long i);

int HyperPage_init (I, Widget parent, const char *title, Any data);

void HyperPage_prefs (void);
void HyperPage_setEntryHint (I, const char *entry);
void HyperPage_initSheetOfPaper (HyperPage me);

/* End of file HyperPage.h */
#endif
