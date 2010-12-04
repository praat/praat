/* HyperPage.c
 *
 * Copyright (C) 1996-2010 Paul Boersma
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
 * pb 2003/09/15 better positioning of buttons
 * pb 2004/11/23 made vertical spacing dependent on font size
 * pb 2005/05/07 embedded scripts (for pictures)
 * pb 2005/07/19 moved "<1" and "1>" buttons to the top, removed horizontal scroll bar and page number
 * pb 2006/10/20 embedded scripts allow links
 * pb 2007/06/10 wchar_t
 * pb 2007/11/30 erased Graphics_printf
 * pb 2008/03/21 new Editor API
 * pb 2008/11/24 prevented crash by Melder_malloc (replaced with Melder_calloc)
 * pb 2009/03/17 split up structPraat
 * pb 2010/05/14 GTK
 */

#include <ctype.h>
#include "HyperPage.h"
#include "Printer.h"
#include "Preferences.h"
#include "machine.h"

#include "praat.h"
#include "EditorM.h"

#define PAGE_HEIGHT  320.0
#define SCREEN_HEIGHT  15.0
#define PAPER_TOP  12.0
#define TOP_MARGIN  0.8
#define PAPER_BOTTOM  (13.0 - (double) thePrinter. paperHeight / thePrinter. resolution)
#define BOTTOM_MARGIN  0.5
static double resolution;

static enum kGraphics_font prefs_font;
static int prefs_fontSize;

void HyperPage_prefs (void) {
	Preferences_addEnum (L"HyperPage.font", & prefs_font, kGraphics_font, DEFAULT);
	Preferences_addInt (L"HyperPage.fontSize", & prefs_fontSize, 12);
}

/********** class HyperLink **********/

class_methods (HyperLink, Data)
class_methods_end

HyperLink HyperLink_create (const wchar_t *name, double x1DC, double x2DC, double y1DC, double y2DC) {
	HyperLink me = new (HyperLink);
	if (! me) return NULL;
	Thing_setName (me, name);
	my x1DC = x1DC, my x2DC = x2DC, my y1DC = y1DC, my y2DC = y2DC;
	return me;
}

static void saveHistory (HyperPage me, const wchar_t *title) {
	if (! title) return;

	/*
	 * The page title will be saved at the top. Go there.
	 */
	while (my historyPointer < 19 && my history [my historyPointer]. page)
		my historyPointer ++;

	/*
	 * If the page title to be saved is already at the top, ignore it.
	 */	
	if (my history [my historyPointer]. page) {
		if (wcsequ (my history [my historyPointer]. page, title)) return;
	} else if (my historyPointer > 0 && wcsequ (my history [my historyPointer - 1]. page, title)) {
		my historyPointer --;
		return;
	}

	/*
	 * If the history buffer is full, shift it.
	 */
	if (my historyPointer == 19 && my history [my historyPointer]. page) {
		int i;
		Melder_free (my history [0]. page);
		for (i = 0; i < 19; i ++) my history [i] = my history [i + 1];
	}

	/*
	 * Add the page title to the top of the history list.
	 */
	my history [my historyPointer]. page = Melder_wcsdup (title);
}

/********************************************************************************
 *
 * Before drawing or printing.
 *
 */

static void initScreen (HyperPage me) {
	my y = PAGE_HEIGHT + my top / 5.0;
	my x = 0;
	my previousBottomSpacing = 0.0;
	forget (my links);
	my links = Collection_create (classHyperLink, 100);
}

void HyperPage_initSheetOfPaper (HyperPage me) {
	int reflect = my mirror && (my pageNumber & 1) == 0;
	wchar_t *leftHeader = reflect ? my outsideHeader : my insideHeader;
	wchar_t *rightHeader = reflect ? my insideHeader : my outsideHeader;
	wchar_t *leftFooter = reflect ? my outsideFooter : my insideFooter;
	wchar_t *rightFooter = reflect ? my insideFooter : my outsideFooter;

	my y = PAPER_TOP - TOP_MARGIN;
	my x = 0;
	my previousBottomSpacing = 0.0;
	Graphics_setFont (my ps, kGraphics_font_TIMES);
	Graphics_setFontSize (my ps, 12);
	Graphics_setFontStyle (my ps, Graphics_ITALIC);
	if (leftHeader) {
		Graphics_setTextAlignment (my ps, Graphics_LEFT, Graphics_TOP);
		Graphics_text (my ps, 0.7, PAPER_TOP, leftHeader);
	}
	if (my middleHeader) {
		Graphics_setTextAlignment (my ps, Graphics_CENTRE, Graphics_TOP);
		Graphics_text (my ps, 0.7 + 3, PAPER_TOP, my middleHeader);
	}
	if (rightHeader) {
		Graphics_setTextAlignment (my ps, Graphics_RIGHT, Graphics_TOP);
		Graphics_text (my ps, 0.7 + 6, PAPER_TOP, rightHeader);
	}
	if (leftFooter) {
		Graphics_setTextAlignment (my ps, Graphics_LEFT, Graphics_BOTTOM);
		Graphics_text (my ps, 0.7, PAPER_BOTTOM, leftFooter);
	}
	if (my middleFooter) {
		Graphics_setTextAlignment (my ps, Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_text (my ps, 0.7 + 3, PAPER_BOTTOM, my middleFooter);
	}
	if (rightFooter) {
		Graphics_setTextAlignment (my ps, Graphics_RIGHT, Graphics_BOTTOM);
		Graphics_text (my ps, 0.7 + 6, PAPER_BOTTOM, rightFooter);
	}
	Graphics_setFontStyle (my ps, Graphics_NORMAL);
	if (my pageNumber)
		Graphics_text1 (my ps, 0.7 + ( reflect ? 0 : 6 ), PAPER_BOTTOM, Melder_integer (my pageNumber));
	Graphics_setTextAlignment (my ps, Graphics_LEFT, Graphics_BOTTOM);
}

static void updateVerticalScrollBar (HyperPage me);

int HyperPage_any (I, const wchar_t *text, int font, int size, int style, double minFooterDistance,
	double x, double secondIndent, double topSpacing, double bottomSpacing, unsigned long method)
{
	iam (HyperPage);
	double heightGuess;

	if (my rightMargin == 0) return 0;
	// Melder_assert (my rightMargin != 0);

	heightGuess = size * (1.2/72) * ((long) size * wcslen (text) / (int) (my rightMargin * 150));

if (! my printing) {
	Graphics_Link *paragraphLinks;
	int numberOfParagraphLinks, ilink;
	if (my entryHint && (method & HyperPage_USE_ENTRY_HINT) && wcsequ (text, my entryHint)) {
		my entryPosition = my y;
	}
	my y -= ( my previousBottomSpacing > topSpacing ? my previousBottomSpacing : topSpacing ) * size / 12.0;
	my y -= size * (1.2/72);
	my x = x;

	if (/* my y > PAGE_HEIGHT + 2.0 + heightGuess || */ my y < PAGE_HEIGHT - SCREEN_HEIGHT) {
		my y -= heightGuess;
	} else {
		Graphics_setFont (my g, font);
		Graphics_setFontSize (my g, size);
		Graphics_setWrapWidth (my g, my rightMargin - x - 0.1);
		Graphics_setSecondIndent (my g, secondIndent);
		Graphics_setFontStyle (my g, style);
		Graphics_text (my g, my x, my y, text);
		numberOfParagraphLinks = Graphics_getLinks (& paragraphLinks);
		if (my links) for (ilink = 1; ilink <= numberOfParagraphLinks; ilink ++) {
			HyperLink link = HyperLink_create (paragraphLinks [ilink]. name,
				paragraphLinks [ilink]. x1, paragraphLinks [ilink]. x2,
				paragraphLinks [ilink]. y1, paragraphLinks [ilink]. y2);
			Collection_addItem (my links, link);
		}
		if (method & HyperPage_ADD_BORDER) {
			Graphics_setLineWidth (my g, 2);
			Graphics_line (my g, 0.0, my y, my rightMargin, my y);
			Graphics_setLineWidth (my g, 1);
		}
		/*
		 * The text may have wrapped.
		 * Ask the Graphics manager by how much, and update our text position accordingly.
		 */
		my y = Graphics_inqTextY (my g);
	}
} else {
	Graphics_setFont (my ps, font);
	Graphics_setFontSize (my ps, size);
	my y -= my y == PAPER_TOP - TOP_MARGIN ? 0 : ( my previousBottomSpacing > topSpacing ? my previousBottomSpacing : topSpacing ) * size / 12.0;
	my y -= size * (1.2/72);
	if (my y < PAPER_BOTTOM + BOTTOM_MARGIN + minFooterDistance + size * (1.2/72) * (wcslen (text) / (6.0 * 10))) {
		Graphics_nextSheetOfPaper (my ps);
		if (my pageNumber) my pageNumber ++;
		HyperPage_initSheetOfPaper (me);
		Graphics_setFont (my ps, font);
		Graphics_setFontSize (my ps, size);
		my y -= size * (1.2/72);
	}
	my x = 0.7 + x;
	Graphics_setWrapWidth (my ps, 6.0 - x);
	Graphics_setSecondIndent (my ps, secondIndent);
	Graphics_setFontStyle (my ps, style);
	Graphics_text (my ps, my x, my y, text);
	if (method & HyperPage_ADD_BORDER) {
		Graphics_setLineWidth (my ps, 3);
		/*Graphics_line (my ps, 0.7, my y, 6.7, my y);*/
		Graphics_line (my ps, 0.7, my y + size * (1.2/72) + 0.07, 6.7, my y + size * (1.2/72) + 0.07);
		Graphics_setLineWidth (my ps, 1);
	}
	my y = Graphics_inqTextY (my ps);
}
	my previousBottomSpacing = bottomSpacing;
	return 1;
}

int HyperPage_pageTitle (I, const wchar_t *title) {
	iam (HyperPage);
	return HyperPage_any (me, title, my font, my fontSize * 2, 0,
		2.0, 0.0, 0.0, my printing ? 0.4/2 : 0.2/2, 0.3/2, HyperPage_ADD_BORDER);
}
int HyperPage_intro (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.0, 0.03, 0.0, 0.1, 0.1, 0);
}
int HyperPage_entry (I, const wchar_t *title) {
	iam (HyperPage);
	return HyperPage_any (me, title, my font, my fontSize * 1.4, Graphics_BOLD, 0.5, 0.0, 0.0, 0.25/1.4, 0.1/1.4, HyperPage_USE_ENTRY_HINT);
}
int HyperPage_paragraph (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.0, 0.03, 0.0, 0.1, 0.1, 0);
}
int HyperPage_listItem (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.0, 0.30, 0.2, 0.0, 0.0, 0);
}
int HyperPage_listItem1 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.0, 0.57, 0.2, 0.0, 0.0, 0);
}
int HyperPage_listItem2 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.0, 0.84, 0.2, 0.0, 0.0, 0);
}
int HyperPage_listItem3 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.0, 1.11, 0.2, 0.0, 0.0, 0);
}
int HyperPage_listTag (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.2, 0.03, 0.0, 0.1, 0.03, 0);
}
int HyperPage_listTag1 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.2, 0.50, 0.0, 0.05, 0.03, 0);
}
int HyperPage_listTag2 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.2, 0.97, 0.0, 0.03, 0.03, 0);
}
int HyperPage_listTag3 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.2, 1.44, 0.0, 0.03, 0.03, 0);
}
int HyperPage_definition (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.0, 0.5, 0.0, 0.03, 0.1, 0);
}
int HyperPage_definition1 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.0, 0.97, 0.0, 0.03, 0.05, 0);
}
int HyperPage_definition2 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.0, 1.44, 0.0, 0.03, 0.03, 0);
}
int HyperPage_definition3 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.0, 1.93, 0.0, 0.03, 0.03, 0);
}
int HyperPage_code (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, kGraphics_font_COURIER, my fontSize * 0.86, 0, 0.0, 0.3, 0.5, 0.0, 0.0, 0);
}
int HyperPage_code1 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, kGraphics_font_COURIER, my fontSize * 0.86, 0, 0.0, 0.6, 0.5, 0.0, 0.0, 0);
}
int HyperPage_code2 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, kGraphics_font_COURIER, my fontSize * 0.86, 0, 0.0, 0.9, 0.5, 0.0, 0.0, 0);
}
int HyperPage_code3 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, kGraphics_font_COURIER, my fontSize * 0.86, 0, 0.0, 1.2, 0.5, 0.0, 0.0, 0);
}
int HyperPage_code4 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, kGraphics_font_COURIER, my fontSize * 0.86, 0, 0.0, 1.5, 0.5, 0.0, 0.0, 0);
}
int HyperPage_code5 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, kGraphics_font_COURIER, my fontSize * 0.86, 0, 0.0, 1.8, 0.5, 0.0, 0.0, 0);
}
int HyperPage_prototype (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.0, 0.03, 0.5, 0.0, 0.0, 0);
}
int HyperPage_formula (I, const wchar_t *formula) {
	iam (HyperPage);
	double topSpacing = 0.2, bottomSpacing = 0.2, minFooterDistance = 0.0;
	int font = my font, size = my fontSize;
if (! my printing) {
	my y -= ( my previousBottomSpacing > topSpacing ? my previousBottomSpacing : topSpacing ) * size / 12.0;
	my y -= size * (1.2/72);
	if (my y > PAGE_HEIGHT + 2.0 || my y < PAGE_HEIGHT - SCREEN_HEIGHT) {
	} else {
		Graphics_setFont (my g, font);
		Graphics_setFontStyle (my g, 0);
		Graphics_setFontSize (my g, size);
		Graphics_setWrapWidth (my g, 0);
		Graphics_setTextAlignment (my g, Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_text (my g, my rightMargin / 2, my y, formula);
		Graphics_setTextAlignment (my g, Graphics_LEFT, Graphics_BOTTOM);
	}
} else {
	Graphics_setFont (my ps, font);
	Graphics_setFontStyle (my ps, 0);
	Graphics_setFontSize (my ps, size);
	my y -= my y == PAPER_TOP - TOP_MARGIN ? 0 : ( my previousBottomSpacing > topSpacing ? my previousBottomSpacing : topSpacing ) * size / 12.0;
	my y -= size * (1.2/72);
	if (my y < PAPER_BOTTOM + BOTTOM_MARGIN + minFooterDistance) {
		Graphics_nextSheetOfPaper (my ps);
		if (my pageNumber) my pageNumber ++;
		HyperPage_initSheetOfPaper (me);
		Graphics_setFont (my ps, font);
		Graphics_setFontSize (my ps, size);
		my y -= size * (1.2/72);
	}
	Graphics_setWrapWidth (my ps, 0);
	Graphics_setTextAlignment (my ps, Graphics_CENTRE, Graphics_BOTTOM);
	Graphics_text (my ps, 3.7, my y, formula);
	Graphics_setTextAlignment (my ps, Graphics_LEFT, Graphics_BOTTOM);
}
	my previousBottomSpacing = bottomSpacing;
	return 1;
}

int HyperPage_picture (I, double width_inches, double height_inches, void (*draw) (Graphics g)) {
	iam (HyperPage);
	double topSpacing = 0.1, bottomSpacing = 0.1, minFooterDistance = 0.0;
	int font = my font, size = my fontSize;
	width_inches *= width_inches < 0.0 ? -1.0 : size / 12.0;
	height_inches *= height_inches < 0.0 ? -1.0 : size / 12.0;
if (! my printing) {
	my y -= ( my previousBottomSpacing > topSpacing ? my previousBottomSpacing : topSpacing ) * size / 12.0;
	if (my y > PAGE_HEIGHT + height_inches || my y < PAGE_HEIGHT - SCREEN_HEIGHT) {
		my y -= height_inches;
	} else {
		my y -= height_inches;
		Graphics_setFont (my g, font);
		Graphics_setFontStyle (my g, 0);
		Graphics_setFontSize (my g, size);
		my x = width_inches > my rightMargin ? 0 : 0.5 * (my rightMargin - width_inches);
		Graphics_setWrapWidth (my g, 0);
		Graphics_setViewport (my g, my x, my x + width_inches, my y, my y + height_inches);
		draw (my g);
		Graphics_setViewport (my g, 0, 1, 0, 1);
		Graphics_setWindow (my g, 0, 1, 0, 1);
		Graphics_setTextAlignment (my g, Graphics_LEFT, Graphics_BOTTOM);
	}
} else {
	Graphics_setFont (my ps, font);
	Graphics_setFontStyle (my ps, 0);
	Graphics_setFontSize (my ps, size);
	my y -= my y == PAPER_TOP - TOP_MARGIN ? 0 : ( my previousBottomSpacing > topSpacing ? my previousBottomSpacing : topSpacing ) * size / 12.0;
	my y -= height_inches;
	if (my y < PAPER_BOTTOM + BOTTOM_MARGIN + minFooterDistance) {
		Graphics_nextSheetOfPaper (my ps);
		if (my pageNumber) my pageNumber ++;
		HyperPage_initSheetOfPaper (me);
		Graphics_setFont (my ps, font);
		Graphics_setFontSize (my ps, size);
		my y -= height_inches;
	}
	my x = 3.7 - 0.5 * width_inches;
	if (my x < 0) my x = 0;
	Graphics_setWrapWidth (my ps, 0);
	Graphics_setViewport (my ps, my x, my x + width_inches, my y, my y + height_inches);
	draw (my ps);
	Graphics_setViewport (my ps, 0, 1, 0, 1);
	Graphics_setWindow (my ps, 0, 1, 0, 1);
	Graphics_setTextAlignment (my ps, Graphics_LEFT, Graphics_BOTTOM);
}
	my previousBottomSpacing = bottomSpacing;
	return 1;
}

int HyperPage_script (I, double width_inches, double height_inches, const wchar_t *script) {
	iam (HyperPage);
	wchar_t *text = Melder_wcsdup (script);
	Interpreter interpreter = Interpreter_createFromEnvironment (NULL);
	double topSpacing = 0.1, bottomSpacing = 0.1, minFooterDistance = 0.0;
	int font = my font, size = my fontSize;
	width_inches *= width_inches < 0.0 ? -1.0 : size / 12.0;
	height_inches *= height_inches < 0.0 ? -1.0 : size / 12.0;
if (! my printing) {
	my y -= ( my previousBottomSpacing > topSpacing ? my previousBottomSpacing : topSpacing ) * size / 12.0;
	if (my y > PAGE_HEIGHT + height_inches || my y < PAGE_HEIGHT - SCREEN_HEIGHT) {
		my y -= height_inches;
	} else {
		my y -= height_inches;
		Graphics_setFont (my g, font);
		Graphics_setFontStyle (my g, 0);
		Graphics_setFontSize (my g, size);
		my x = width_inches > my rightMargin ? 0 : 0.5 * (my rightMargin - width_inches);
		Graphics_setWrapWidth (my g, 0);
		{
			if (my praatApplication == NULL) my praatApplication = Melder_calloc (structPraatApplication, 1);
			if (my praatObjects == NULL) my praatObjects = Melder_calloc (structPraatObjects, 1);
			if (my praatPicture == NULL) my praatPicture = Melder_calloc (structPraatPicture, 1);
			theCurrentPraatApplication = my praatApplication;
			theCurrentPraatApplication -> batch = true;   // prevent creation of editor windows
			theCurrentPraatApplication -> topShell = theForegroundPraatApplication. topShell;   // needed for UiForm_create () in dialogs
			theCurrentPraatObjects = my praatObjects;
			theCurrentPraatPicture = my praatPicture;
			theCurrentPraatPicture -> graphics = my g;   // has to draw into HyperPage rather than Picture window
			theCurrentPraatPicture -> font = font;
			theCurrentPraatPicture -> fontSize = size;
			theCurrentPraatPicture -> lineType = Graphics_DRAWN;
			theCurrentPraatPicture -> colour = Graphics_BLACK;
			theCurrentPraatPicture -> lineWidth = 1.0;
			theCurrentPraatPicture -> arrowSize = 1.0;
			theCurrentPraatPicture -> x1NDC = my x;
			theCurrentPraatPicture -> x2NDC = my x + width_inches;
			theCurrentPraatPicture -> y1NDC = my y;
			theCurrentPraatPicture -> y2NDC = my y + height_inches;
			Graphics_setViewport (my g, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
			Melder_progressOff ();
			Melder_warningOff ();
			structMelderDir saveDir = { { 0 } };
			Melder_getDefaultDir (& saveDir);
			if (! MelderDir_isNull (& my rootDirectory)) Melder_setDefaultDir (& my rootDirectory);
			Interpreter_run (interpreter, text);
			Melder_setDefaultDir (& saveDir);
			Melder_warningOn ();
			Melder_progressOn ();
			Graphics_setLineType (my g, Graphics_DRAWN);
			Graphics_setLineWidth (my g, 1.0);
			Graphics_setArrowSize (my g, 1.0);
			Graphics_setColour (my g, Graphics_BLACK);
			iferror {
				if (my scriptErrorHasBeenNotified) {
					Melder_clearError ();
				} else {
					Melder_flushError (NULL);
					my scriptErrorHasBeenNotified = true;
				}
			}
			/*Graphics_Link *paragraphLinks;
			long numberOfParagraphLinks = Graphics_getLinks (& paragraphLinks);
			if (my links) for (long ilink = 1; ilink <= numberOfParagraphLinks; ilink ++) {
				HyperLink link = HyperLink_create (paragraphLinks [ilink]. name,
					paragraphLinks [ilink]. x1, paragraphLinks [ilink]. x2,
					paragraphLinks [ilink]. y1, paragraphLinks [ilink]. y2);
				Collection_addItem (my links, link);
			}*/
			theCurrentPraatApplication = & theForegroundPraatApplication;
			theCurrentPraatObjects = & theForegroundPraatObjects;
			theCurrentPraatPicture = & theForegroundPraatPicture;
		}
		Graphics_setViewport (my g, 0, 1, 0, 1);
		Graphics_setWindow (my g, 0, 1, 0, 1);
		Graphics_setTextAlignment (my g, Graphics_LEFT, Graphics_BOTTOM);
	}
} else {
	Graphics_setFont (my ps, font);
	Graphics_setFontStyle (my ps, 0);
	Graphics_setFontSize (my ps, size);
	my y -= my y == PAPER_TOP - TOP_MARGIN ? 0 : ( my previousBottomSpacing > topSpacing ? my previousBottomSpacing : topSpacing ) * size / 12.0;
	my y -= height_inches;
	if (my y < PAPER_BOTTOM + BOTTOM_MARGIN + minFooterDistance) {
		Graphics_nextSheetOfPaper (my ps);
		if (my pageNumber) my pageNumber ++;
		HyperPage_initSheetOfPaper (me);
		Graphics_setFont (my ps, font);
		Graphics_setFontSize (my ps, size);
		my y -= height_inches;
	}
	my x = 3.7 - 0.5 * width_inches;
	if (my x < 0) my x = 0;
	Graphics_setWrapWidth (my ps, 0);
	{
		if (my praatApplication == NULL) my praatApplication = Melder_calloc (structPraatApplication, 1);
		if (my praatObjects == NULL) my praatObjects = Melder_calloc (structPraatObjects, 1);
		if (my praatPicture == NULL) my praatPicture = Melder_calloc (structPraatPicture, 1);
		theCurrentPraatApplication = my praatApplication;
		theCurrentPraatApplication -> batch = true;
		theCurrentPraatObjects = my praatObjects;
		theCurrentPraatPicture = my praatPicture;
		theCurrentPraatPicture -> graphics = my ps;
		theCurrentPraatPicture -> font = font;
		theCurrentPraatPicture -> fontSize = size;
		theCurrentPraatPicture -> lineType = Graphics_DRAWN;
		theCurrentPraatPicture -> colour = Graphics_BLACK;
		theCurrentPraatPicture -> lineWidth = 1.0;
		theCurrentPraatPicture -> arrowSize = 1.0;
		theCurrentPraatPicture -> x1NDC = my x;
		theCurrentPraatPicture -> x2NDC = my x + width_inches;
		theCurrentPraatPicture -> y1NDC = my y;
		theCurrentPraatPicture -> y2NDC = my y + height_inches;
		Graphics_setViewport (my ps, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
		Melder_progressOff ();
		Melder_warningOff ();
		structMelderDir saveDir = { { 0 } };
		Melder_getDefaultDir (& saveDir);
		if (! MelderDir_isNull (& my rootDirectory)) Melder_setDefaultDir (& my rootDirectory);
		Interpreter_run (interpreter, text);
		Melder_setDefaultDir (& saveDir);
		Melder_warningOn ();
		Melder_progressOn ();
		iferror Melder_clearError ();
		Graphics_setLineType (my ps, Graphics_DRAWN);
		Graphics_setLineWidth (my ps, 1.0);
		Graphics_setArrowSize (my ps, 1.0);
		Graphics_setColour (my ps, Graphics_BLACK);
		theCurrentPraatApplication = & theForegroundPraatApplication;
		theCurrentPraatObjects = & theForegroundPraatObjects;
		theCurrentPraatPicture = & theForegroundPraatPicture;
	}
	Graphics_setViewport (my ps, 0, 1, 0, 1);
	Graphics_setWindow (my ps, 0, 1, 0, 1);
	Graphics_setTextAlignment (my ps, Graphics_LEFT, Graphics_BOTTOM);
}
	my previousBottomSpacing = bottomSpacing;
	forget (interpreter);
	Melder_free (text);
	return 1;
}

static void draw (HyperPage hyperPage) {
	(void) hyperPage;
}

static void print (I, Graphics graphics) {
	iam (HyperPage);
	my ps = graphics;
	Graphics_setDollarSignIsCode (graphics, TRUE);
	Graphics_setAtSignIsLink (graphics, TRUE);
	my printing = TRUE;
	HyperPage_initSheetOfPaper (me);
	our draw (me);
	my printing = FALSE;
}

/********** class HyperPage **********/

static void destroy (I) {
	iam (HyperPage);
	forget (my links);
	Melder_free (my entryHint);
	forget (my g);
	for (int i = 0; i < 20; i ++) Melder_free (my history [i]. page);
	Melder_free (my currentPageTitle);
	if (my praatApplication != NULL) {
		for (int iobject = ((PraatObjects) my praatObjects) -> n; iobject >= 1; iobject --) {
			Melder_free (((PraatObjects) my praatObjects) -> list [iobject]. name);
			forget (((PraatObjects) my praatObjects) -> list [iobject]. object);
		}
		Melder_free (my praatApplication);
		Melder_free (my praatObjects);
		Melder_free (my praatPicture);
	}
	inherited (HyperPage) destroy (me);
}

static void gui_drawingarea_cb_expose (I, GuiDrawingAreaExposeEvent event) {
	iam (HyperPage);
	(void) event;
	if (my g == NULL) return;   // Could be the case in the very beginning.
	Graphics_clearWs (my g);
	initScreen (me);
	our draw (me);
	if (my entryHint && my entryPosition) {
		Melder_free (my entryHint);
		my top = 5.0 * (PAGE_HEIGHT - my entryPosition);
		if (my top < 0) my top = 0;
		Graphics_clearWs (my g);
		initScreen (me);
		our draw (me);
		updateVerticalScrollBar (me);
	}
}

static void gui_drawingarea_cb_click (I, GuiDrawingAreaClickEvent event) {
	iam (HyperPage);
	if (my g == NULL) return;   // Could be the case in the very beginning.
if (gtk && event -> type != BUTTON_PRESS) return;
	if (! my links) return;
	for (long ilink = 1; ilink <= my links -> size; ilink ++) {
		HyperLink link = my links -> item [ilink];
		if (event -> y > link -> y2DC && event -> y < link -> y1DC && event -> x > link -> x1DC && event -> x < link -> x2DC) {
			saveHistory (me, my currentPageTitle);
			if (! HyperPage_goToPage (me, link -> name)) {
				/* Allow for a returned 0 just to mean: 'do not jump'. */
				if (Melder_hasError ()) Melder_flushError (NULL);
			}
			return;
		}
	}
}

static int menu_cb_postScriptSettings (EDITOR_ARGS) {
	EDITOR_IAM (HyperPage);
	Printer_postScriptSettings ();
	return 1;
}

#ifdef macintosh
static int menu_cb_pageSetup (EDITOR_ARGS) {
	EDITOR_IAM (HyperPage);
	Printer_pageSetup ();
	return 1;
}
#endif

static int menu_cb_print (EDITOR_ARGS) {
	EDITOR_IAM (HyperPage);
	EDITOR_FORM (L"Print", 0)
		SENTENCE (L"Left or inside header", L"")
		SENTENCE (L"Middle header", L"")
		LABEL (L"", L"Right or outside header:")
		TEXTFIELD (L"Right or outside header", L"")
		SENTENCE (L"Left or inside footer", L"")
		SENTENCE (L"Middle footer", L"")
		SENTENCE (L"Right or outside footer", L"")
		BOOLEAN (L"Mirror even/odd headers", TRUE)
		INTEGER (L"First page number", L"0 (= no page numbers)")
	EDITOR_OK
		our defaultHeaders (cmd);
		if (my pageNumber) SET_INTEGER (L"First page number", my pageNumber + 1)
	EDITOR_DO
		my insideHeader = GET_STRING (L"Left or inside header");
		my middleHeader = GET_STRING (L"Middle header");
		my outsideHeader = GET_STRING (L"Right or outside header");
		my insideFooter = GET_STRING (L"Left or inside footer");
		my middleFooter = GET_STRING (L"Middle footer");
		my outsideFooter = GET_STRING (L"Right or outside footer");
		my mirror = GET_INTEGER (L"Mirror even/odd headers");
		my pageNumber = GET_INTEGER (L"First page number");
		Printer_print (print, me);
	EDITOR_END
}

static int menu_cb_font (EDITOR_ARGS) {
	EDITOR_IAM (HyperPage);
	EDITOR_FORM (L"Font", 0)
		RADIO (L"Font", 1)
			RADIOBUTTON (L"Times")
			RADIOBUTTON (L"Helvetica")
	EDITOR_OK
		SET_INTEGER (L"Font", my font == kGraphics_font_TIMES ? 1 :
				my font == kGraphics_font_HELVETICA ? 2 : my font == kGraphics_font_PALATINO ? 3 : 1);
	EDITOR_DO
		int font = GET_INTEGER (L"Font");
		prefs_font = my font = font == 1 ? kGraphics_font_TIMES : kGraphics_font_HELVETICA;
		if (my g) Graphics_updateWs (my g);
	EDITOR_END
}

static void updateSizeMenu (HyperPage me) {
	GuiMenuItem_check (my fontSizeButton_10, my fontSize == 10);
	GuiMenuItem_check (my fontSizeButton_12, my fontSize == 12);
	GuiMenuItem_check (my fontSizeButton_14, my fontSize == 14);
	GuiMenuItem_check (my fontSizeButton_18, my fontSize == 18);
	GuiMenuItem_check (my fontSizeButton_24, my fontSize == 24);
}
static void setFontSize (HyperPage me, int fontSize) {
	prefs_fontSize = my fontSize = fontSize;
	if (my g) Graphics_updateWs (my g);
	updateSizeMenu (me);
}

static int menu_cb_10 (EDITOR_ARGS) { EDITOR_IAM (HyperPage); setFontSize (me, 10); return 1; }
static int menu_cb_12 (EDITOR_ARGS) { EDITOR_IAM (HyperPage); setFontSize (me, 12); return 1; }
static int menu_cb_14 (EDITOR_ARGS) { EDITOR_IAM (HyperPage); setFontSize (me, 14); return 1; }
static int menu_cb_18 (EDITOR_ARGS) { EDITOR_IAM (HyperPage); setFontSize (me, 18); return 1; }
static int menu_cb_24 (EDITOR_ARGS) { EDITOR_IAM (HyperPage); setFontSize (me, 24); return 1; }

static int menu_cb_fontSize (EDITOR_ARGS) {
	EDITOR_IAM (HyperPage);
	EDITOR_FORM (L"Font size", 0)
		NATURAL (L"Font size (points)", L"12")
	EDITOR_OK
		SET_INTEGER (L"Font size", my fontSize)
	EDITOR_DO
		setFontSize (me, GET_INTEGER (L"Font size"));
	EDITOR_END
}

static int menu_cb_searchForPage (EDITOR_ARGS) {
	EDITOR_IAM (HyperPage);
	EDITOR_FORM (L"Search for page", 0)
		TEXTFIELD (L"Page", L"a")
	EDITOR_OK
	EDITOR_DO
		if (! HyperPage_goToPage (me, GET_STRING (L"Page"))) return 0;
	EDITOR_END
}

/********************************************************************************
 *
 * The vertical scroll bar controls and/or mirrors
 * the position of the viewable area within the page.
 * A page can be PAGE_HEIGHT inches high, so 'my top' (and the scroll-bar 'value')
 * may take on values between 0 and PAGE_HEIGHT * 5 (fifth inches).
 * Hence, the 'minimum' is 0.
 * The viewable area shows a certain number of fifth inches;
 * hence the 'sliderSize' is height / resolution * 5,
 * and the 'maximum' is PAGE_HEIGHT * 5.
 * The 'increment' is 1, so the arrows move the page by one fifth of an inch.
 * The 'pageIncrement' is sliderSize - 1.
 */

static void createVerticalScrollBar (HyperPage me, GuiObject parent) {
	#if gtk
		int maximumScrollBarValue = (int) (PAGE_HEIGHT * 5);
		GtkObject *adj = gtk_adjustment_new (1, 1, maximumScrollBarValue, 1, 1, maximumScrollBarValue - 1);
		my verticalScrollBar = gtk_vscrollbar_new (GTK_ADJUSTMENT (adj));
		GuiObject_show (my verticalScrollBar);
		gtk_box_pack_end (GTK_BOX (parent), my verticalScrollBar, false, false, 3);
	#elif motif
		// TODO: Kan dit niet een algemele gui klasse worden?
		my verticalScrollBar = XtVaCreateManagedWidget ("verticalScrollBar",
			xmScrollBarWidgetClass, parent, XmNorientation, XmVERTICAL,
			XmNrightAttachment, XmATTACH_FORM,
			XmNtopAttachment, XmATTACH_FORM,
				XmNtopOffset, Machine_getMenuBarHeight () + Machine_getTextHeight () + 12,
			XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, Machine_getScrollBarWidth (),
			XmNwidth, Machine_getScrollBarWidth (),
			XmNminimum, 0, XmNmaximum, (int) (PAGE_HEIGHT * 5),
			XmNsliderSize, 25, XmNvalue, 0,
			XmNincrement, 1, XmNpageIncrement, 24,
			NULL);
	#endif
}

static void updateVerticalScrollBar (HyperPage me)
/* We cannot call this immediately after creation. */
/* This has to be called after changing 'my topParagraph'. */
{
	Dimension width, height;
	int sliderSize;
	#if motif
		XtVaGetValues (my drawingArea, XmNwidth, & width, XmNheight, & height, NULL);
	#endif
	sliderSize = 25 /*height / resolution * 5*/;   /* Don't change slider unless you clip value! */
	#if gtk
		GtkAdjustment *adj = gtk_range_get_adjustment (GTK_RANGE (my verticalScrollBar));
		adj -> page_size = sliderSize;
		//gtk_adjustment_set_value (adj, value);
		gtk_adjustment_changed (adj);
		gtk_range_set_increments (GTK_RANGE (my verticalScrollBar), 1, sliderSize - 1);
	#elif motif
		XmScrollBarSetValues (my verticalScrollBar, my top, sliderSize, 1, sliderSize - 1, False);
	#endif
	my history [my historyPointer]. top = 0/*my top*/;
}

#if gtk
static void gui_cb_verticalScroll (GtkRange *rng, gpointer void_me) {
	iam (HyperPage);
	double value = gtk_range_get_value (GTK_RANGE (rng));
	if (value != my top) {
		my top = value;
		Graphics_clearWs (my g);
		initScreen (me);
		our draw (me);   /* Do not wait for expose event. */
		updateVerticalScrollBar (me);
	}
}
#else
static void gui_cb_verticalScroll (GUI_ARGS) {
	GUI_IAM (HyperPage);
	int value, sliderSize, incr, pincr;
	#if gtk
		double value = gtk_range_get_value (GTK_RANGE (w));
	#elif motif
		XmScrollBarGetValues (w, & value, & sliderSize, & incr, & pincr);
	#endif
	if (value != my top) {
		my top = value;
		Graphics_clearWs (my g);
		initScreen (me);
		our draw (me);   /* Do not wait for expose event. */
		updateVerticalScrollBar (me);
	}
}
#endif

static int menu_cb_pageUp (EDITOR_ARGS) {
	EDITOR_IAM (HyperPage);
	int value, sliderSize, incr, pincr;
	if (! my verticalScrollBar) return 0;
	#if	gtk
		value = gtk_range_get_value (GTK_RANGE (my verticalScrollBar));
		sliderSize = 1;
		pincr = PAGE_HEIGHT * 5 - 1;
	#elif motif
		XmScrollBarGetValues (my verticalScrollBar, & value, & sliderSize, & incr, & pincr);
	#endif
	value -= pincr;
	if (value < 0) value = 0;
	if (value != my top) {
		my top = value;
		Graphics_clearWs (my g);
		initScreen (me);
		our draw (me);   /* Do not wait for expose event. */
		updateVerticalScrollBar (me);
	}
	return 1;
}

static int menu_cb_pageDown (EDITOR_ARGS) {
	EDITOR_IAM (HyperPage);
	int value, sliderSize, incr, pincr;
	if (! my verticalScrollBar) return 0;
	#if	gtk
		value = gtk_range_get_value (GTK_RANGE (my verticalScrollBar));
		sliderSize = 1;
		pincr = PAGE_HEIGHT * 5 - 1;
	#elif motif
		XmScrollBarGetValues (my verticalScrollBar, & value, & sliderSize, & incr, & pincr);
	#endif
	value += pincr;
	if (value > (int) (PAGE_HEIGHT * 5) - sliderSize) value = (int) (PAGE_HEIGHT * 5) - sliderSize;
	if (value != my top) {
		my top = value;
		Graphics_clearWs (my g);
		initScreen (me);
		our draw (me);   /* Do not wait for expose event. */
		updateVerticalScrollBar (me);
	}
	return 1;
}

/********** **********/

static int do_back (HyperPage me) {
	if (my historyPointer <= 0) return 1;
	wchar_t *page = Melder_wcsdup (my history [-- my historyPointer]. page);   /* Temporary, because pointer will be moved. */
	int top = my history [my historyPointer]. top;
	if (our goToPage (me, page)) {
		my top = top;
		HyperPage_clear (me);
		updateVerticalScrollBar (me);
	} else {
		Melder_free (page);
		return 0;
	}
	Melder_free (page);
	return 1;
}

static int menu_cb_back (EDITOR_ARGS) {
	EDITOR_IAM (HyperPage);
	if (! do_back (me)) return 0;
	return 1;
}

static void gui_button_cb_back (I, GuiButtonEvent event) {
	(void) event;
	iam (HyperPage);
	if (! do_back (me)) Melder_flushError (NULL);
}

static int do_forth (HyperPage me) {
	wchar_t *page;
	int top;
	if (my historyPointer >= 19 || ! my history [my historyPointer + 1]. page) return 1;
	page = Melder_wcsdup (my history [++ my historyPointer]. page);
	top = my history [my historyPointer]. top;
	if (our goToPage (me, page)) {
		my top = top;
		HyperPage_clear (me);
		updateVerticalScrollBar (me);
	} else {
		Melder_free (page);
		return 0;
	}
	Melder_free (page);
	return 1;
}

static int menu_cb_forth (EDITOR_ARGS) {
	EDITOR_IAM (HyperPage);
	if (! do_forth (me)) return 0;
	return 1;
}

static void gui_button_cb_forth (I, GuiButtonEvent event) {
	(void) event;
	iam (HyperPage);
	if (! do_forth (me)) Melder_flushError (NULL);
}

static void createMenus (HyperPage me) {
	inherited (HyperPage) createMenus (HyperPage_as_parent (me));

	Editor_addCommand (me, L"File", L"PostScript settings...", 0, menu_cb_postScriptSettings);
	#ifdef macintosh
		Editor_addCommand (me, L"File", L"Page setup...", 0, menu_cb_pageSetup);
	#endif
	Editor_addCommand (me, L"File", L"Print page...", 'P', menu_cb_print);
	Editor_addCommand (me, L"File", L"-- close --", 0, NULL);

	if (our hasHistory) {
		Editor_addMenu (me, L"Go to", 0);
		Editor_addCommand (me, L"Go to", L"Search for page...", 0, menu_cb_searchForPage);
		Editor_addCommand (me, L"Go to", L"Back", GuiMenu_OPTION | GuiMenu_LEFT_ARROW, menu_cb_back);
		Editor_addCommand (me, L"Go to", L"Forward", GuiMenu_OPTION | GuiMenu_RIGHT_ARROW, menu_cb_forth);
		Editor_addCommand (me, L"Go to", L"-- page --", 0, NULL);
		Editor_addCommand (me, L"Go to", L"Page up", GuiMenu_PAGE_UP, menu_cb_pageUp);
		Editor_addCommand (me, L"Go to", L"Page down", GuiMenu_PAGE_DOWN, menu_cb_pageDown);
	}

	Editor_addMenu (me, L"Font", 0);
	Editor_addCommand (me, L"Font", L"Font size...", 0, menu_cb_fontSize);
	my fontSizeButton_10 = Editor_addCommand (me, L"Font", L"10", GuiMenu_CHECKBUTTON, menu_cb_10);
	my fontSizeButton_12 = Editor_addCommand (me, L"Font", L"12", GuiMenu_CHECKBUTTON, menu_cb_12);
	my fontSizeButton_14 = Editor_addCommand (me, L"Font", L"14", GuiMenu_CHECKBUTTON, menu_cb_14);
	my fontSizeButton_18 = Editor_addCommand (me, L"Font", L"18", GuiMenu_CHECKBUTTON, menu_cb_18);
	my fontSizeButton_24 = Editor_addCommand (me, L"Font", L"24", GuiMenu_CHECKBUTTON, menu_cb_24);
	Editor_addCommand (me, L"Font", L"-- font --", 0, NULL);
	Editor_addCommand (me, L"Font", L"Font...", 0, menu_cb_font);
}

/********** **********/

static void gui_drawingarea_cb_resize (I, GuiDrawingAreaResizeEvent event) {
	iam (HyperPage);
	if (my g == NULL) return;
	Graphics_setWsViewport (my g, 0, event -> width, 0, event -> height);
	Graphics_setWsWindow (my g, 0.0, my rightMargin = event -> width / resolution,
		PAGE_HEIGHT - event -> height / resolution, PAGE_HEIGHT);
	Graphics_updateWs (my g);
	updateVerticalScrollBar (me);
}

static void gui_button_cb_previousPage (I, GuiButtonEvent event) {
	(void) event;
	iam (HyperPage);
	HyperPage_goToPage_i (me, our getCurrentPageNumber (me) > 1 ?
		our getCurrentPageNumber (me) - 1 : our getNumberOfPages (me));
}

static void gui_button_cb_nextPage (I, GuiButtonEvent event) {
	(void) event;
	iam (HyperPage);
	HyperPage_goToPage_i (me, our getCurrentPageNumber (me) < our getNumberOfPages (me) ?
		our getCurrentPageNumber (me) + 1 : 1);
}

static void createChildren (HyperPage me) {
	int height = Machine_getTextHeight ();
	int y = Machine_getMenuBarHeight () + 4;

	#if gtk
		my holder = gtk_hbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (my dialog), my holder, false, false, 0);
		GuiObject_show (my holder);
	#elif motif
		my holder = my dialog;
	#endif

	/***** Create navigation buttons. *****/

	if (our hasHistory) {
		GuiButton_createShown (my holder, 4, 48, y, y + height,
			L"<", gui_button_cb_back, me, 0);
		GuiButton_createShown (my holder, 54, 98, y, y + height,
			L">", gui_button_cb_forth, me, 0);
	}
	if (our isOrdered) {
		GuiButton_createShown (my holder, 174, 218, y, y + height,
			L"< 1", gui_button_cb_previousPage, me, 0);
		GuiButton_createShown (my holder, 224, 268, y, y + height,
			L"1 >", gui_button_cb_nextPage, me, 0);
	}
	#if gtk
		GuiObject scrollBox = gtk_hbox_new (false, 0);
		gtk_box_pack_end (GTK_BOX (my dialog), scrollBox, true, true, 0);
		my drawingArea = GuiDrawingArea_create (GTK_WIDGET (scrollBox), 0, 600, 0, 800,
			gui_drawingarea_cb_expose, gui_drawingarea_cb_click, NULL, gui_drawingarea_cb_resize, me, GuiDrawingArea_BORDER);
		gtk_widget_set_double_buffered (my drawingArea, FALSE);
		gtk_box_pack_start (GTK_BOX (scrollBox), my drawingArea, true, true, 0);
		createVerticalScrollBar (me, scrollBox);
		GuiObject_show (my drawingArea);
		GuiObject_show (scrollBox);
	#elif motif
		/***** Create scroll bar. *****/

		createVerticalScrollBar (me, my dialog);

		/***** Create drawing area. *****/
		my drawingArea = GuiDrawingArea_createShown (my dialog, 0, - Machine_getScrollBarWidth (), y + height + 8, - Machine_getScrollBarWidth (),
			gui_drawingarea_cb_expose, gui_drawingarea_cb_click, NULL, gui_drawingarea_cb_resize, me, GuiDrawingArea_BORDER);
	#endif
}

int HyperPage_init (HyperPage me, GuiObject parent, const wchar_t *title, Any data) {
	resolution = Gui_getResolution (parent);
	Editor_init (HyperPage_as_parent (me), parent, 0, 0, 6 * resolution + 30, 800, title, data); cherror
	#if motif
		Melder_assert (XtWindow (my drawingArea));
	#endif
	my g = Graphics_create_xmdrawingarea (my drawingArea);
	Graphics_setAtSignIsLink (my g, TRUE);
	Graphics_setDollarSignIsCode (my g, TRUE);
	Graphics_setFont (my g, kGraphics_font_TIMES);
	if (prefs_font != kGraphics_font_TIMES && prefs_font != kGraphics_font_HELVETICA)
		prefs_font = kGraphics_font_TIMES;   // Ensure Unicode compatibility.
	my font = prefs_font;
	setFontSize (me, prefs_fontSize);	

struct structGuiDrawingAreaResizeEvent event = { my drawingArea, 0 };
event. width = GuiObject_getWidth (my drawingArea);
event. height = GuiObject_getHeight (my drawingArea);
gui_drawingarea_cb_resize (me, & event);

	#if gtk
		g_signal_connect (G_OBJECT (my verticalScrollBar), "value-changed", G_CALLBACK (gui_cb_verticalScroll), me);
	#elif motif
		XtAddCallback (my verticalScrollBar, XmNvalueChangedCallback, gui_cb_verticalScroll, (XtPointer) me);
		XtAddCallback (my verticalScrollBar, XmNdragCallback, gui_cb_verticalScroll, (XtPointer) me);
	#endif
	updateVerticalScrollBar (me);   /* Scroll to the top (my top == 0). */
end:
	iferror return 0;
	return 1;
}

void HyperPage_clear (HyperPage me) {
	Graphics_updateWs (my g);
	forget (my links);
}

static void dataChanged (HyperPage me) {
	int oldError = Melder_hasError ();
	(void) our goToPage (me, my currentPageTitle);
	if (Melder_hasError () && ! oldError) Melder_flushError (NULL);
	HyperPage_clear (me);
	updateVerticalScrollBar (me);
}
static long getNumberOfPages (HyperPage me) {
	(void) me;
	return 0;
}
static long getCurrentPageNumber (HyperPage me) {
	(void) me;
	return 0;
}
static void defaultHeaders (EditorCommand cmd) {
	(void) cmd;
}
static int goToPage (HyperPage me, const wchar_t *title) {
	(void) me;
	(void) title;
	return 0;
}
static int goToPage_i (HyperPage me, long i) {
	(void) me;
	(void) i;
	return 0;
}
static int hasHistory = FALSE;
static int isOrdered = FALSE;

class_methods (HyperPage, Editor) {
	class_method (destroy)
	class_method (dataChanged)
	class_method (draw)
	us -> editable = false;
	class_method (createMenus)
	class_method (createChildren)
	class_method (defaultHeaders)
	class_method (getNumberOfPages)
	class_method (getCurrentPageNumber)
	class_method (goToPage)
	class_method (goToPage_i)
	class_method (hasHistory)
	class_method (isOrdered)
	class_methods_end
}

int HyperPage_goToPage (I, const wchar_t *title) {
	iam (HyperPage);
	switch (our goToPage (me, title)) {
		case -1: return 0;
		case 0: HyperPage_clear (me); return 0;
	}
	saveHistory (me, title);   /* Last chance: HyperPage_clear will destroy "title" !!! */
	Melder_free (my currentPageTitle);
	my currentPageTitle = Melder_wcsdup (title);
	my top = 0;
	HyperPage_clear (me);
	updateVerticalScrollBar (me);   /* Scroll to the top (my top == 0). */
	return 1;	
}

int HyperPage_goToPage_i (I, long i) {
	iam (HyperPage);
	if (! our goToPage_i (me, i)) { HyperPage_clear (me); return 0; }
	my top = 0;
	HyperPage_clear (me);
	updateVerticalScrollBar (me);   /* Scroll to the top (my top == 0). */
	return 1;
}

void HyperPage_setEntryHint (I, const wchar_t *hint) {
	iam (HyperPage);
	Melder_free (my entryHint);
	my entryHint = Melder_wcsdup (hint);
}

/* End of file HyperPage.c */
