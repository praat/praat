/* HyperPage.cpp
 *
 * Copyright (C) 1996-2011,2012,2013 Paul Boersma
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

#include <ctype.h>
#include "HyperPage.h"
#include "Printer.h"
#include "machine.h"
#include "GuiP.h"

#include "praat.h"
#include "EditorM.h"

Thing_implement (HyperPage, Editor, 0);

#include "prefs_define.h"
#include "HyperPage_prefs.h"
#include "prefs_install.h"
#include "HyperPage_prefs.h"
#include "prefs_copyToInstance.h"
#include "HyperPage_prefs.h"

#define PAGE_HEIGHT  320.0
#define SCREEN_HEIGHT  15.0
#define PAPER_TOP  12.0
#define TOP_MARGIN  0.8
#define PAPER_BOTTOM  (13.0 - (double) thePrinter. paperHeight / thePrinter. resolution)
#define BOTTOM_MARGIN  0.5
static double resolution;

/********** class HyperLink **********/

Thing_implement (HyperLink, Data, 0);

HyperLink HyperLink_create (const wchar_t *name, double x1DC, double x2DC, double y1DC, double y2DC) {
	autoHyperLink me = Thing_new (HyperLink);
	Thing_setName (me.peek(), name);
	my x1DC = x1DC, my x2DC = x2DC, my y1DC = y1DC, my y2DC = y2DC;
	return me.transfer();
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
	my history [my historyPointer]. page = Melder_wcsdup_f (title);
}

/********************************************************************************
 *
 * Before drawing or printing.
 *
 */

static void initScreen (HyperPage me) {
	my d_y = PAGE_HEIGHT + my top / 5.0;
	my d_x = 0;
	my previousBottomSpacing = 0.0;
	forget (my links);
	my links = Collection_create (classHyperLink, 100);
}

void HyperPage_initSheetOfPaper (HyperPage me) {
	int reflect = my mirror && (my d_printingPageNumber & 1) == 0;
	wchar_t *leftHeader = reflect ? my outsideHeader : my insideHeader;
	wchar_t *rightHeader = reflect ? my insideHeader : my outsideHeader;
	wchar_t *leftFooter = reflect ? my outsideFooter : my insideFooter;
	wchar_t *rightFooter = reflect ? my insideFooter : my outsideFooter;

	my d_y = PAPER_TOP - TOP_MARGIN;
	my d_x = 0;
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
	if (my d_printingPageNumber)
		Graphics_text1 (my ps, 0.7 + ( reflect ? 0 : 6 ), PAPER_BOTTOM, Melder_integer (my d_printingPageNumber));
	Graphics_setTextAlignment (my ps, Graphics_LEFT, Graphics_BOTTOM);
}

static void updateVerticalScrollBar (HyperPage me);

int HyperPage_any (I, const wchar_t *text, enum kGraphics_font font, int size, int style, double minFooterDistance,
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
		my entryPosition = my d_y;
	}
	my d_y -= ( my previousBottomSpacing > topSpacing ? my previousBottomSpacing : topSpacing ) * size / 12.0;
	my d_y -= size * (1.2/72);
	my d_x = x;

	if (/* my d_y > PAGE_HEIGHT + 2.0 + heightGuess || */ my d_y < PAGE_HEIGHT - SCREEN_HEIGHT) {
		my d_y -= heightGuess;
	} else {
		Graphics_setFont (my g, font);
		Graphics_setFontSize (my g, size);
		Graphics_setWrapWidth (my g, my rightMargin - x - 0.1);
		Graphics_setSecondIndent (my g, secondIndent);
		Graphics_setFontStyle (my g, style);
		Graphics_text (my g, my d_x, my d_y, text);
		numberOfParagraphLinks = Graphics_getLinks (& paragraphLinks);
		if (my links) for (ilink = 1; ilink <= numberOfParagraphLinks; ilink ++) {
			HyperLink link = HyperLink_create (paragraphLinks [ilink]. name,
				paragraphLinks [ilink]. x1, paragraphLinks [ilink]. x2,
				paragraphLinks [ilink]. y1, paragraphLinks [ilink]. y2);
			Collection_addItem (my links, link);
		}
		if (method & HyperPage_ADD_BORDER) {
			Graphics_setLineWidth (my g, 2);
			Graphics_line (my g, 0.0, my d_y, my rightMargin, my d_y);
			Graphics_setLineWidth (my g, 1);
		}
		/*
		 * The text may have wrapped.
		 * Ask the Graphics manager by how much, and update our text position accordingly.
		 */
		my d_y = Graphics_inqTextY (my g);
	}
} else {
	Graphics_setFont (my ps, font);
	Graphics_setFontSize (my ps, size);
	my d_y -= my d_y == PAPER_TOP - TOP_MARGIN ? 0 : ( my previousBottomSpacing > topSpacing ? my previousBottomSpacing : topSpacing ) * size / 12.0;
	my d_y -= size * (1.2/72);
	if (my d_y < PAPER_BOTTOM + BOTTOM_MARGIN + minFooterDistance + size * (1.2/72) * (wcslen (text) / (6.0 * 10))) {
		Graphics_nextSheetOfPaper (my ps);
		if (my d_printingPageNumber) my d_printingPageNumber ++;
		HyperPage_initSheetOfPaper (me);
		Graphics_setFont (my ps, font);
		Graphics_setFontSize (my ps, size);
		my d_y -= size * (1.2/72);
	}
	my d_x = 0.7 + x;
	Graphics_setWrapWidth (my ps, 6.0 - x);
	Graphics_setSecondIndent (my ps, secondIndent);
	Graphics_setFontStyle (my ps, style);
	Graphics_text (my ps, my d_x, my d_y, text);
	if (method & HyperPage_ADD_BORDER) {
		Graphics_setLineWidth (my ps, 3);
		/*Graphics_line (my ps, 0.7, my d_y, 6.7, my d_y);*/
		Graphics_line (my ps, 0.7, my d_y + size * (1.2/72) + 0.07, 6.7, my d_y + size * (1.2/72) + 0.07);
		Graphics_setLineWidth (my ps, 1);
	}
	my d_y = Graphics_inqTextY (my ps);
}
	my previousBottomSpacing = bottomSpacing;
	return 1;
}

int HyperPage_pageTitle (I, const wchar_t *title) {
	iam (HyperPage);
	return HyperPage_any (me, title, my p_font, my p_fontSize * 2, 0,
		2.0, 0.0, 0.0, my printing ? 0.4/2 : 0.2/2, 0.3/2, HyperPage_ADD_BORDER);
}
int HyperPage_intro (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my p_font, my p_fontSize, 0, 0.0, 0.03, 0.0, 0.1, 0.1, 0);
}
int HyperPage_entry (I, const wchar_t *title) {
	iam (HyperPage);
	return HyperPage_any (me, title, my p_font, my p_fontSize * 1.4, Graphics_BOLD, 0.5, 0.0, 0.0, 0.25/1.4, 0.1/1.4, HyperPage_USE_ENTRY_HINT);
}
int HyperPage_paragraph (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my p_font, my p_fontSize, 0, 0.0, 0.03, 0.0, 0.1, 0.1, 0);
}
int HyperPage_listItem (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my p_font, my p_fontSize, 0, 0.0, 0.30, 0.2, 0.0, 0.0, 0);
}
int HyperPage_listItem1 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my p_font, my p_fontSize, 0, 0.0, 0.57, 0.2, 0.0, 0.0, 0);
}
int HyperPage_listItem2 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my p_font, my p_fontSize, 0, 0.0, 0.84, 0.2, 0.0, 0.0, 0);
}
int HyperPage_listItem3 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my p_font, my p_fontSize, 0, 0.0, 1.11, 0.2, 0.0, 0.0, 0);
}
int HyperPage_listTag (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my p_font, my p_fontSize, 0, 0.2, 0.03, 0.0, 0.1, 0.03, 0);
}
int HyperPage_listTag1 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my p_font, my p_fontSize, 0, 0.2, 0.50, 0.0, 0.05, 0.03, 0);
}
int HyperPage_listTag2 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my p_font, my p_fontSize, 0, 0.2, 0.97, 0.0, 0.03, 0.03, 0);
}
int HyperPage_listTag3 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my p_font, my p_fontSize, 0, 0.2, 1.44, 0.0, 0.03, 0.03, 0);
}
int HyperPage_definition (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my p_font, my p_fontSize, 0, 0.0, 0.5, 0.0, 0.03, 0.1, 0);
}
int HyperPage_definition1 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my p_font, my p_fontSize, 0, 0.0, 0.97, 0.0, 0.03, 0.05, 0);
}
int HyperPage_definition2 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my p_font, my p_fontSize, 0, 0.0, 1.44, 0.0, 0.03, 0.03, 0);
}
int HyperPage_definition3 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my p_font, my p_fontSize, 0, 0.0, 1.93, 0.0, 0.03, 0.03, 0);
}
int HyperPage_code (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, kGraphics_font_COURIER, my p_fontSize * 0.86, 0, 0.0, 0.3, 0.5, 0.0, 0.0, 0);
}
int HyperPage_code1 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, kGraphics_font_COURIER, my p_fontSize * 0.86, 0, 0.0, 0.6, 0.5, 0.0, 0.0, 0);
}
int HyperPage_code2 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, kGraphics_font_COURIER, my p_fontSize * 0.86, 0, 0.0, 0.9, 0.5, 0.0, 0.0, 0);
}
int HyperPage_code3 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, kGraphics_font_COURIER, my p_fontSize * 0.86, 0, 0.0, 1.2, 0.5, 0.0, 0.0, 0);
}
int HyperPage_code4 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, kGraphics_font_COURIER, my p_fontSize * 0.86, 0, 0.0, 1.5, 0.5, 0.0, 0.0, 0);
}
int HyperPage_code5 (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, kGraphics_font_COURIER, my p_fontSize * 0.86, 0, 0.0, 1.8, 0.5, 0.0, 0.0, 0);
}
int HyperPage_prototype (I, const wchar_t *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my p_font, my p_fontSize, 0, 0.0, 0.03, 0.5, 0.0, 0.0, 0);
}
int HyperPage_formula (I, const wchar_t *formula) {
	iam (HyperPage);
	double topSpacing = 0.2, bottomSpacing = 0.2, minFooterDistance = 0.0;
	kGraphics_font font = my p_font;
	int size = my p_fontSize;
if (! my printing) {
	my d_y -= ( my previousBottomSpacing > topSpacing ? my previousBottomSpacing : topSpacing ) * size / 12.0;
	my d_y -= size * (1.2/72);
	if (my d_y > PAGE_HEIGHT + 2.0 || my d_y < PAGE_HEIGHT - SCREEN_HEIGHT) {
	} else {
		Graphics_setFont (my g, font);
		Graphics_setFontStyle (my g, 0);
		Graphics_setFontSize (my g, size);
		Graphics_setWrapWidth (my g, 0);
		Graphics_setTextAlignment (my g, Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_text (my g, my rightMargin / 2, my d_y, formula);
		Graphics_setTextAlignment (my g, Graphics_LEFT, Graphics_BOTTOM);
	}
} else {
	Graphics_setFont (my ps, font);
	Graphics_setFontStyle (my ps, 0);
	Graphics_setFontSize (my ps, size);
	my d_y -= my d_y == PAPER_TOP - TOP_MARGIN ? 0 : ( my previousBottomSpacing > topSpacing ? my previousBottomSpacing : topSpacing ) * size / 12.0;
	my d_y -= size * (1.2/72);
	if (my d_y < PAPER_BOTTOM + BOTTOM_MARGIN + minFooterDistance) {
		Graphics_nextSheetOfPaper (my ps);
		if (my d_printingPageNumber) my d_printingPageNumber ++;
		HyperPage_initSheetOfPaper (me);
		Graphics_setFont (my ps, font);
		Graphics_setFontSize (my ps, size);
		my d_y -= size * (1.2/72);
	}
	Graphics_setWrapWidth (my ps, 0);
	Graphics_setTextAlignment (my ps, Graphics_CENTRE, Graphics_BOTTOM);
	Graphics_text (my ps, 3.7, my d_y, formula);
	Graphics_setTextAlignment (my ps, Graphics_LEFT, Graphics_BOTTOM);
}
	my previousBottomSpacing = bottomSpacing;
	return 1;
}

int HyperPage_picture (I, double width_inches, double height_inches, void (*draw) (Graphics g)) {
	iam (HyperPage);
	double topSpacing = 0.1, bottomSpacing = 0.1, minFooterDistance = 0.0;
	kGraphics_font font = my p_font;
	int size = my p_fontSize;
	width_inches *= width_inches < 0.0 ? -1.0 : size / 12.0;
	height_inches *= height_inches < 0.0 ? -1.0 : size / 12.0;
if (! my printing) {
	my d_y -= ( my previousBottomSpacing > topSpacing ? my previousBottomSpacing : topSpacing ) * size / 12.0;
	if (my d_y > PAGE_HEIGHT + height_inches || my d_y < PAGE_HEIGHT - SCREEN_HEIGHT) {
		my d_y -= height_inches;
	} else {
		my d_y -= height_inches;
		Graphics_setFont (my g, font);
		Graphics_setFontStyle (my g, 0);
		Graphics_setFontSize (my g, size);
		my d_x = width_inches > my rightMargin ? 0 : 0.5 * (my rightMargin - width_inches);
		Graphics_setWrapWidth (my g, 0);
		Graphics_setViewport (my g, my d_x, my d_x + width_inches, my d_y, my d_y + height_inches);
		draw (my g);
		Graphics_setViewport (my g, 0, 1, 0, 1);
		Graphics_setWindow (my g, 0, 1, 0, 1);
		Graphics_setTextAlignment (my g, Graphics_LEFT, Graphics_BOTTOM);
	}
} else {
	Graphics_setFont (my ps, font);
	Graphics_setFontStyle (my ps, 0);
	Graphics_setFontSize (my ps, size);
	my d_y -= my d_y == PAPER_TOP - TOP_MARGIN ? 0 : ( my previousBottomSpacing > topSpacing ? my previousBottomSpacing : topSpacing ) * size / 12.0;
	my d_y -= height_inches;
	if (my d_y < PAPER_BOTTOM + BOTTOM_MARGIN + minFooterDistance) {
		Graphics_nextSheetOfPaper (my ps);
		if (my d_printingPageNumber) my d_printingPageNumber ++;
		HyperPage_initSheetOfPaper (me);
		Graphics_setFont (my ps, font);
		Graphics_setFontSize (my ps, size);
		my d_y -= height_inches;
	}
	my d_x = 3.7 - 0.5 * width_inches;
	if (my d_x < 0) my d_x = 0;
	Graphics_setWrapWidth (my ps, 0);
	Graphics_setViewport (my ps, my d_x, my d_x + width_inches, my d_y, my d_y + height_inches);
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
	wchar_t *text = Melder_wcsdup_f (script);
	Interpreter interpreter = Interpreter_createFromEnvironment (NULL);
	double topSpacing = 0.1, bottomSpacing = 0.1, minFooterDistance = 0.0;
	kGraphics_font font = my p_font;
	int size = my p_fontSize;
	double true_width_inches = width_inches * ( width_inches < 0.0 ? -1.0 : size / 12.0 );
	double true_height_inches = height_inches * ( height_inches < 0.0 ? -1.0 : size / 12.0 );
if (! my printing) {
	my d_y -= ( my previousBottomSpacing > topSpacing ? my previousBottomSpacing : topSpacing ) * size / 12.0;
	if (my d_y > PAGE_HEIGHT + true_height_inches || my d_y < PAGE_HEIGHT - SCREEN_HEIGHT) {
		my d_y -= true_height_inches;
	} else {
		my d_y -= true_height_inches;
		Graphics_setFont (my g, font);
		Graphics_setFontStyle (my g, 0);
		Graphics_setFontSize (my g, size);
		my d_x = true_width_inches > my rightMargin ? 0 : 0.5 * (my rightMargin - true_width_inches);
		Graphics_setWrapWidth (my g, 0);
		long x1DCold, x2DCold, y1DCold, y2DCold;
		Graphics_inqWsViewport (my g, & x1DCold, & x2DCold, & y1DCold, & y2DCold);
		double x1NDCold, x2NDCold, y1NDCold, y2NDCold;
		Graphics_inqWsWindow (my g, & x1NDCold, & x2NDCold, & y1NDCold, & y2NDCold);
		{
			if (my praatApplication == NULL) my praatApplication = Melder_calloc_f (structPraatApplication, 1);
			if (my praatObjects == NULL) my praatObjects = Melder_calloc_f (structPraatObjects, 1);
			if (my praatPicture == NULL) my praatPicture = Melder_calloc_f (structPraatPicture, 1);
			theCurrentPraatApplication = (PraatApplication) my praatApplication;
			theCurrentPraatApplication -> batch = true;   // prevent creation of editor windows
			theCurrentPraatApplication -> topShell = theForegroundPraatApplication. topShell;   // needed for UiForm_create () in dialogs
			theCurrentPraatObjects = (PraatObjects) my praatObjects;
			theCurrentPraatPicture = (PraatPicture) my praatPicture;
			theCurrentPraatPicture -> graphics = my g;   // has to draw into HyperPage rather than Picture window
			theCurrentPraatPicture -> font = font;
			theCurrentPraatPicture -> fontSize = size;
			theCurrentPraatPicture -> lineType = Graphics_DRAWN;
			theCurrentPraatPicture -> colour = Graphics_BLACK;
			theCurrentPraatPicture -> lineWidth = 1.0;
			theCurrentPraatPicture -> arrowSize = 1.0;
			theCurrentPraatPicture -> x1NDC = my d_x;
			theCurrentPraatPicture -> x2NDC = my d_x + true_width_inches;
			theCurrentPraatPicture -> y1NDC = my d_y;
			theCurrentPraatPicture -> y2NDC = my d_y + true_height_inches;

			Graphics_setViewport (my g, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
			Graphics_setWindow (my g, 0.0, 1.0, 0.0, 1.0);
			long x1DC, y1DC, x2DC, y2DC;
			Graphics_WCtoDC (my g, 0.0, 0.0, & x1DC, & y2DC);
			Graphics_WCtoDC (my g, 1.0, 1.0, & x2DC, & y1DC);
			Graphics_resetWsViewport (my g, x1DC, x2DC, y1DC, y2DC);
			Graphics_setWsWindow (my g, 0, width_inches, 0, height_inches);
			theCurrentPraatPicture -> x1NDC = 0;
			theCurrentPraatPicture -> x2NDC = width_inches;
			theCurrentPraatPicture -> y1NDC = 0;
			theCurrentPraatPicture -> y2NDC = height_inches;
			Graphics_setViewport (my g, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);			

			{// scope
				autoMelderProgressOff progress;
				autoMelderWarningOff warning;
				autoMelderSaveDefaultDir saveDir;
				if (! MelderDir_isNull (& my rootDirectory)) {
					Melder_setDefaultDir (& my rootDirectory);
				}
				try {
					Interpreter_run (interpreter, text);
				} catch (MelderError) {
					if (my scriptErrorHasBeenNotified) {
						Melder_clearError ();
					} else {
						Melder_flushError (NULL);
						my scriptErrorHasBeenNotified = true;
					}
				}
			}
			Graphics_setLineType (my g, Graphics_DRAWN);
			Graphics_setLineWidth (my g, 1.0);
			Graphics_setArrowSize (my g, 1.0);
			Graphics_setColour (my g, Graphics_BLACK);
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
		Graphics_resetWsViewport (my g, x1DCold, x2DCold, y1DCold, y2DCold);
		Graphics_setWsWindow (my g, x1NDCold, x2NDCold, y1NDCold, y2NDCold);
		Graphics_setViewport (my g, 0, 1, 0, 1);
		Graphics_setWindow (my g, 0, 1, 0, 1);
		Graphics_setTextAlignment (my g, Graphics_LEFT, Graphics_BOTTOM);
	}
} else {
	Graphics_setFont (my ps, font);
	Graphics_setFontStyle (my ps, 0);
	Graphics_setFontSize (my ps, size);
	my d_y -= my d_y == PAPER_TOP - TOP_MARGIN ? 0 : ( my previousBottomSpacing > topSpacing ? my previousBottomSpacing : topSpacing ) * size / 12.0;
	my d_y -= true_height_inches;
	if (my d_y < PAPER_BOTTOM + BOTTOM_MARGIN + minFooterDistance) {
		Graphics_nextSheetOfPaper (my ps);
		if (my d_printingPageNumber) my d_printingPageNumber ++;
		HyperPage_initSheetOfPaper (me);
		Graphics_setFont (my ps, font);
		Graphics_setFontSize (my ps, size);
		my d_y -= true_height_inches;
	}
	my d_x = 3.7 - 0.5 * true_width_inches;
	if (my d_x < 0) my d_x = 0;
	Graphics_setWrapWidth (my ps, 0);
	long x1DCold, x2DCold, y1DCold, y2DCold;
	Graphics_inqWsViewport (my ps, & x1DCold, & x2DCold, & y1DCold, & y2DCold);
	double x1NDCold, x2NDCold, y1NDCold, y2NDCold;
	Graphics_inqWsWindow (my ps, & x1NDCold, & x2NDCold, & y1NDCold, & y2NDCold);
	{
		if (my praatApplication == NULL) my praatApplication = Melder_calloc_f (structPraatApplication, 1);
		if (my praatObjects == NULL) my praatObjects = Melder_calloc_f (structPraatObjects, 1);
		if (my praatPicture == NULL) my praatPicture = Melder_calloc_f (structPraatPicture, 1);
		theCurrentPraatApplication = (PraatApplication) my praatApplication;
		theCurrentPraatApplication -> batch = true;
		theCurrentPraatApplication -> topShell = theForegroundPraatApplication. topShell;   // needed for UiForm_create () in dialogs
		theCurrentPraatObjects = (PraatObjects) my praatObjects;
		theCurrentPraatPicture = (PraatPicture) my praatPicture;
		theCurrentPraatPicture -> graphics = my ps;
		theCurrentPraatPicture -> font = font;
		theCurrentPraatPicture -> fontSize = size;
		theCurrentPraatPicture -> lineType = Graphics_DRAWN;
		theCurrentPraatPicture -> colour = Graphics_BLACK;
		theCurrentPraatPicture -> lineWidth = 1.0;
		theCurrentPraatPicture -> arrowSize = 1.0;
		theCurrentPraatPicture -> x1NDC = my d_x;
		theCurrentPraatPicture -> x2NDC = my d_x + true_width_inches;
		theCurrentPraatPicture -> y1NDC = my d_y;
		theCurrentPraatPicture -> y2NDC = my d_y + true_height_inches;

		Graphics_setViewport (my ps, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
		Graphics_setWindow (my ps, 0.0, 1.0, 0.0, 1.0);
		long x1DC, y1DC, x2DC, y2DC;
		Graphics_WCtoDC (my ps, 0.0, 0.0, & x1DC, & y2DC);
		Graphics_WCtoDC (my ps, 1.0, 1.0, & x2DC, & y1DC);
		long shift = (long) (Graphics_getResolution (my ps) * true_height_inches) + (y1DCold - y2DCold);
		Graphics_resetWsViewport (my ps, x1DC, x2DC, y1DC + shift, y2DC + shift);
		Graphics_setWsWindow (my ps, 0, width_inches, 0, height_inches);
		theCurrentPraatPicture -> x1NDC = 0;
		theCurrentPraatPicture -> x2NDC = width_inches;
		theCurrentPraatPicture -> y1NDC = 0;
		theCurrentPraatPicture -> y2NDC = height_inches;
		Graphics_setViewport (my ps, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);

		{// scope
			autoMelderProgressOff progress;
			autoMelderWarningOff warning;
			autoMelderSaveDefaultDir saveDir;
			if (! MelderDir_isNull (& my rootDirectory)) {
				Melder_setDefaultDir (& my rootDirectory);
			}
			try {
				Interpreter_run (interpreter, text);
			} catch (MelderError) {
				Melder_clearError ();
			}
		}
		Graphics_setLineType (my ps, Graphics_DRAWN);
		Graphics_setLineWidth (my ps, 1.0);
		Graphics_setArrowSize (my ps, 1.0);
		Graphics_setColour (my ps, Graphics_BLACK);
		theCurrentPraatApplication = & theForegroundPraatApplication;
		theCurrentPraatObjects = & theForegroundPraatObjects;
		theCurrentPraatPicture = & theForegroundPraatPicture;
	}
	Graphics_resetWsViewport (my ps, x1DCold, x2DCold, y1DCold, y2DCold);
	Graphics_setWsWindow (my ps, x1NDCold, x2NDCold, y1NDCold, y2NDCold);
	Graphics_setViewport (my ps, 0, 1, 0, 1);
	Graphics_setWindow (my ps, 0, 1, 0, 1);
	Graphics_setTextAlignment (my ps, Graphics_LEFT, Graphics_BOTTOM);
}
	my previousBottomSpacing = bottomSpacing;
	forget (interpreter);
	Melder_free (text);
	return 1;
}

static void print (I, Graphics graphics) {
	iam (HyperPage);
	my ps = graphics;
	Graphics_setDollarSignIsCode (graphics, TRUE);
	Graphics_setAtSignIsLink (graphics, TRUE);
	my printing = TRUE;
	HyperPage_initSheetOfPaper (me);
	my v_draw ();
	my printing = FALSE;
}

/********** class HyperPage **********/

void structHyperPage :: v_destroy () {
	forget (links);
	Melder_free (entryHint);
	forget (g);
	for (int i = 0; i < 20; i ++) Melder_free (history [i]. page);
	Melder_free (currentPageTitle);
	if (praatApplication != NULL) {
		for (int iobject = ((PraatObjects) praatObjects) -> n; iobject >= 1; iobject --) {
			Melder_free (((PraatObjects) praatObjects) -> list [iobject]. name);
			forget (((PraatObjects) praatObjects) -> list [iobject]. object);
		}
		Melder_free (praatApplication);
		Melder_free (praatObjects);
		Melder_free (praatPicture);
	}
	HyperPage_Parent :: v_destroy ();
}

static void gui_drawingarea_cb_expose (I, GuiDrawingAreaExposeEvent event) {
	iam (HyperPage);
	(void) event;
	if (my g == NULL) return;   // Could be the case in the very beginning.
	Graphics_clearWs (my g);
	initScreen (me);
	trace ("going to draw");
	my v_draw ();
	if (my entryHint && my entryPosition) {
		Melder_free (my entryHint);
		my top = 5.0 * (PAGE_HEIGHT - my entryPosition);
		if (my top < 0) my top = 0;
		Graphics_clearWs (my g);
		initScreen (me);
		my v_draw ();
		updateVerticalScrollBar (me);
	}
}

static void gui_drawingarea_cb_click (I, GuiDrawingAreaClickEvent event) {
	iam (HyperPage);
	if (my g == NULL) return;   // Could be the case in the very beginning.
	if (! my links) return;
	for (long ilink = 1; ilink <= my links -> size; ilink ++) {
		HyperLink link = (HyperLink) my links -> item [ilink];		
		if (link == NULL)
			Melder_fatal ("gui_drawingarea_cb_click: empty link %ld/%ld.", ilink, my links -> size);
		if (event -> y > link -> y2DC && event -> y < link -> y1DC && event -> x > link -> x1DC && event -> x < link -> x2DC) {
			saveHistory (me, my currentPageTitle);
			try {
				HyperPage_goToPage (me, link -> name);
			} catch (MelderError) {
				Melder_flushError (NULL);
			}
			return;
		}
	}
}

static void menu_cb_postScriptSettings (EDITOR_ARGS) {
	EDITOR_IAM (HyperPage);
	Printer_postScriptSettings ();
}

#ifdef macintosh
static void menu_cb_pageSetup (EDITOR_ARGS) {
	EDITOR_IAM (HyperPage);
	Printer_pageSetup ();
}
#endif

static void menu_cb_print (EDITOR_ARGS) {
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
		my v_defaultHeaders (cmd);
		if (my d_printingPageNumber) SET_INTEGER (L"First page number", my d_printingPageNumber + 1)
	EDITOR_DO
		my insideHeader = GET_STRING (L"Left or inside header");
		my middleHeader = GET_STRING (L"Middle header");
		my outsideHeader = GET_STRING (L"Right or outside header");
		my insideFooter = GET_STRING (L"Left or inside footer");
		my middleFooter = GET_STRING (L"Middle footer");
		my outsideFooter = GET_STRING (L"Right or outside footer");
		my mirror = GET_INTEGER (L"Mirror even/odd headers");
		my d_printingPageNumber = GET_INTEGER (L"First page number");
		Printer_print (print, me);
	EDITOR_END
}

static void menu_cb_font (EDITOR_ARGS) {
	EDITOR_IAM (HyperPage);
	EDITOR_FORM (L"Font", 0)
		RADIO (L"Font", 1)
			RADIOBUTTON (L"Times")
			RADIOBUTTON (L"Helvetica")
	EDITOR_OK
		SET_INTEGER (L"Font", my p_font == kGraphics_font_TIMES ? 1 :
				my p_font == kGraphics_font_HELVETICA ? 2 : my p_font == kGraphics_font_PALATINO ? 3 : 1);
	EDITOR_DO
		int font = GET_INTEGER (L"Font");
		my pref_font () = my p_font = font == 1 ? kGraphics_font_TIMES : kGraphics_font_HELVETICA;
		if (my g) Graphics_updateWs (my g);
	EDITOR_END
}

static void updateSizeMenu (HyperPage me) {
	my fontSizeButton_10 -> f_check (my p_fontSize == 10);
	my fontSizeButton_12 -> f_check (my p_fontSize == 12);
	my fontSizeButton_14 -> f_check (my p_fontSize == 14);
	my fontSizeButton_18 -> f_check (my p_fontSize == 18);
	my fontSizeButton_24 -> f_check (my p_fontSize == 24);
}
static void setFontSize (HyperPage me, int fontSize) {
	my pref_fontSize () = my p_fontSize = fontSize;
	if (my g) Graphics_updateWs (my g);
	updateSizeMenu (me);
}

static void menu_cb_10 (EDITOR_ARGS) { EDITOR_IAM (HyperPage); setFontSize (me, 10); }
static void menu_cb_12 (EDITOR_ARGS) { EDITOR_IAM (HyperPage); setFontSize (me, 12); }
static void menu_cb_14 (EDITOR_ARGS) { EDITOR_IAM (HyperPage); setFontSize (me, 14); }
static void menu_cb_18 (EDITOR_ARGS) { EDITOR_IAM (HyperPage); setFontSize (me, 18); }
static void menu_cb_24 (EDITOR_ARGS) { EDITOR_IAM (HyperPage); setFontSize (me, 24); }

static void menu_cb_fontSize (EDITOR_ARGS) {
	EDITOR_IAM (HyperPage);
	EDITOR_FORM (L"Font size", 0)
		NATURAL (L"Font size (points)", my default_fontSize ())
	EDITOR_OK
		SET_INTEGER (L"Font size", my p_fontSize)
	EDITOR_DO
		setFontSize (me, GET_INTEGER (L"Font size"));
	EDITOR_END
}

static void menu_cb_searchForPage (EDITOR_ARGS) {
	EDITOR_IAM (HyperPage);
	EDITOR_FORM (L"Search for page", 0)
		TEXTFIELD (L"Page", L"a")
	EDITOR_OK
	EDITOR_DO
		HyperPage_goToPage (me, GET_STRING (L"Page"));   // BUG
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

static void gui_cb_verticalScroll (I, GuiScrollBarEvent	event) {
	iam (HyperPage);
	double value = event -> scrollBar -> f_getValue ();
	if (value != my top) {
		trace ("scroll from %f to %f", (double) my top, value);
		my top = value;
		#if cocoa || gtk || win
			Graphics_updateWs (my g);   // wait for expose event
		#else
			initScreen (me);
			Graphics_clearWs (my g);
			my v_draw ();   // do not wait for expose event
		#endif
		updateVerticalScrollBar (me);
	}
}

static void createVerticalScrollBar (HyperPage me, GuiForm parent) {
	my verticalScrollBar = GuiScrollBar_createShown (parent,
		- Machine_getScrollBarWidth (), 0,
		Machine_getMenuBarHeight () + Machine_getTextHeight () + 12, - Machine_getScrollBarWidth (),
		0, PAGE_HEIGHT * 5, 0, 25, 1, 24,
		gui_cb_verticalScroll, me, 0);
}

static void updateVerticalScrollBar (HyperPage me)
/* We cannot call this immediately after creation. */
/* This has to be called after changing 'my topParagraph'. */
{
	int sliderSize = 25;
	my verticalScrollBar -> f_set (NUMundefined, NUMundefined, my top, sliderSize, 1, sliderSize - 1);
	my history [my historyPointer]. top = 0/*my top*/;
}

static void menu_cb_pageUp (EDITOR_ARGS) {
	EDITOR_IAM (HyperPage);
	if (! my verticalScrollBar) return;
	int value = my verticalScrollBar -> f_getValue () - 24;
	if (value < 0) value = 0;
	if (value != my top) {
		my top = value;
		Graphics_clearWs (my g);
		initScreen (me);
		my v_draw ();   // do not wait for expose event
		updateVerticalScrollBar (me);
	}
}

static void menu_cb_pageDown (EDITOR_ARGS) {
	EDITOR_IAM (HyperPage);
	if (! my verticalScrollBar) return;
	int value = my verticalScrollBar -> f_getValue () + 24;
	if (value > (int) (PAGE_HEIGHT * 5) - 25) value = (int) (PAGE_HEIGHT * 5) - 25;
	if (value != my top) {
		my top = value;
		Graphics_clearWs (my g);
		initScreen (me);
		my v_draw ();   // do not wait for expose event
		updateVerticalScrollBar (me);
	}
}

/********** **********/

static void do_back (HyperPage me) {
	if (my historyPointer <= 0) return;
	autostring page = Melder_wcsdup_f (my history [-- my historyPointer]. page);   // temporary, because pointer will be moved
	int top = my history [my historyPointer]. top;
	if (my v_goToPage (page.peek())) {
		my top = top;
		HyperPage_clear (me);
		updateVerticalScrollBar (me);
	}
}

static void menu_cb_back (EDITOR_ARGS) {
	EDITOR_IAM (HyperPage);
	do_back (me);
}

static void gui_button_cb_back (I, GuiButtonEvent event) {
	(void) event;
	iam (HyperPage);
	do_back (me);
}

static void do_forth (HyperPage me) {
	if (my historyPointer >= 19 || ! my history [my historyPointer + 1]. page) return;
	autostring page = Melder_wcsdup_f (my history [++ my historyPointer]. page);
	int top = my history [my historyPointer]. top;
	if (my v_goToPage (page.peek())) {
		my top = top;
		HyperPage_clear (me);
		updateVerticalScrollBar (me);
	}
}

static void menu_cb_forth (EDITOR_ARGS) {
	EDITOR_IAM (HyperPage);
	do_forth (me);
}

static void gui_button_cb_forth (I, GuiButtonEvent event) {
	(void) event;
	iam (HyperPage);
	do_forth (me);
}

void structHyperPage :: v_createMenus () {
	HyperPage_Parent :: v_createMenus ();

	Editor_addCommand (this, L"File", L"PostScript settings...", 0, menu_cb_postScriptSettings);
	#ifdef macintosh
		Editor_addCommand (this, L"File", L"Page setup...", 0, menu_cb_pageSetup);
	#endif
	Editor_addCommand (this, L"File", L"Print page...", 'P', menu_cb_print);
	Editor_addCommand (this, L"File", L"-- close --", 0, NULL);

	if (v_hasHistory ()) {
		Editor_addMenu (this, L"Go to", 0);
		Editor_addCommand (this, L"Go to", L"Search for page...", 0, menu_cb_searchForPage);
		Editor_addCommand (this, L"Go to", L"Back", GuiMenu_OPTION | GuiMenu_LEFT_ARROW, menu_cb_back);
		Editor_addCommand (this, L"Go to", L"Forward", GuiMenu_OPTION | GuiMenu_RIGHT_ARROW, menu_cb_forth);
		Editor_addCommand (this, L"Go to", L"-- page --", 0, NULL);
		Editor_addCommand (this, L"Go to", L"Page up", GuiMenu_PAGE_UP, menu_cb_pageUp);
		Editor_addCommand (this, L"Go to", L"Page down", GuiMenu_PAGE_DOWN, menu_cb_pageDown);
	}

	Editor_addMenu (this, L"Font", 0);
	Editor_addCommand (this, L"Font", L"Font size...", 0, menu_cb_fontSize);
	fontSizeButton_10 = Editor_addCommand (this, L"Font", L"10", GuiMenu_CHECKBUTTON, menu_cb_10);
	fontSizeButton_12 = Editor_addCommand (this, L"Font", L"12", GuiMenu_CHECKBUTTON, menu_cb_12);
	fontSizeButton_14 = Editor_addCommand (this, L"Font", L"14", GuiMenu_CHECKBUTTON, menu_cb_14);
	fontSizeButton_18 = Editor_addCommand (this, L"Font", L"18", GuiMenu_CHECKBUTTON, menu_cb_18);
	fontSizeButton_24 = Editor_addCommand (this, L"Font", L"24", GuiMenu_CHECKBUTTON, menu_cb_24);
	Editor_addCommand (this, L"Font", L"-- font --", 0, NULL);
	Editor_addCommand (this, L"Font", L"Font...", 0, menu_cb_font);
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
	HyperPage_goToPage_i (me, my v_getCurrentPageNumber () > 1 ?
		my v_getCurrentPageNumber () - 1 : my v_getNumberOfPages ());
}

static void gui_button_cb_nextPage (I, GuiButtonEvent event) {
	(void) event;
	iam (HyperPage);
	long currentPageNumber = my v_getCurrentPageNumber ();
	HyperPage_goToPage_i (me, currentPageNumber < my v_getNumberOfPages () ? currentPageNumber + 1 : 1);
}

void structHyperPage :: v_createChildren () {
	int height = Machine_getTextHeight ();
	int y = Machine_getMenuBarHeight () + 4;

	/***** Create navigation buttons. *****/

	if (v_hasHistory ()) {
		GuiButton_createShown (d_windowForm, 4, 48, y, y + height,
			L"<", gui_button_cb_back, this, 0);
		GuiButton_createShown (d_windowForm, 54, 98, y, y + height,
			L">", gui_button_cb_forth, this, 0);
	}
	if (v_isOrdered ()) {
		GuiButton_createShown (d_windowForm, 174, 218, y, y + height,
			L"< 1", gui_button_cb_previousPage, this, 0);
		GuiButton_createShown (d_windowForm, 224, 268, y, y + height,
			L"1 >", gui_button_cb_nextPage, this, 0);
	}

	/***** Create scroll bar. *****/

	createVerticalScrollBar (this, d_windowForm);

	/***** Create drawing area. *****/

	drawingArea = GuiDrawingArea_createShown (d_windowForm, 0, - Machine_getScrollBarWidth (), y + height + 9, - Machine_getScrollBarWidth (),
		gui_drawingarea_cb_expose, gui_drawingarea_cb_click, NULL, gui_drawingarea_cb_resize, this, GuiDrawingArea_BORDER);
	drawingArea -> f_setSwipable (NULL, verticalScrollBar);
}

void HyperPage_init (HyperPage me, const wchar_t *title, Data data) {
	resolution = Gui_getResolution (NULL);
	Editor_init (me, 0, 0, 6 * resolution + 30, 800, title, data);
	#if motif
		Melder_assert (XtWindow (my drawingArea -> d_widget));
	#endif
	my g = Graphics_create_xmdrawingarea (my drawingArea);
	Graphics_setAtSignIsLink (my g, TRUE);
	Graphics_setDollarSignIsCode (my g, TRUE);
	Graphics_setFont (my g, kGraphics_font_TIMES);
	if (my p_font != kGraphics_font_TIMES && my p_font != kGraphics_font_HELVETICA)
		my pref_font () = my p_font = kGraphics_font_TIMES;   // ensure Unicode compatibility
	setFontSize (me, my p_fontSize);

struct structGuiDrawingAreaResizeEvent event = { my drawingArea, 0 };
event. width  = my drawingArea -> f_getWidth  ();
event. height = my drawingArea -> f_getHeight ();
gui_drawingarea_cb_resize (me, & event);

	updateVerticalScrollBar (me);   // scroll to the top (my top == 0)
}

void HyperPage_clear (HyperPage me) {
	Graphics_updateWs (my g);
	forget (my links);
}

void structHyperPage :: v_dataChanged () {
	int oldError = Melder_hasError ();   // this method can be called during error time
	(void) v_goToPage (currentPageTitle);
	if (Melder_hasError () && ! oldError) Melder_flushError (NULL);
	HyperPage_clear (this);
	updateVerticalScrollBar (this);
}

int HyperPage_goToPage (I, const wchar_t *title) {
	iam (HyperPage);
	switch (my v_goToPage (title)) {
		case -1: return 0;
		case 0: HyperPage_clear (me); return 0;
	}
	saveHistory (me, title);   /* Last chance: HyperPage_clear will destroy "title" !!! */
	Melder_free (my currentPageTitle);
	my currentPageTitle = Melder_wcsdup_f (title);
	my top = 0;
	HyperPage_clear (me);
	updateVerticalScrollBar (me);   /* Scroll to the top (my top == 0). */
	return 1;	
}

void HyperPage_goToPage_i (I, long i) {
	iam (HyperPage);
	my v_goToPage_i (i);   // catch -> HyperPage_clear (me); ?
	my top = 0;
	HyperPage_clear (me);
	updateVerticalScrollBar (me);   /* Scroll to the top (my top == 0). */
}

void HyperPage_setEntryHint (I, const wchar_t *hint) {
	iam (HyperPage);
	Melder_free (my entryHint);
	my entryHint = Melder_wcsdup_f (hint);
}

/* End of file HyperPage.cpp */
