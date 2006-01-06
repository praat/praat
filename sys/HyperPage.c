/* HyperPage.c
 *
 * Copyright (C) 1996-2005 Paul Boersma
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
 * pb 2005/05/07 script
 * pb 2005/07/19 moved "<1" and "1>" buttons to the top, removed horizontal scroll bar and page number
 */

#include <ctype.h>
#include "HyperPage.h"
#include "Printer.h"
#include "longchar.h"
#include "Preferences.h"
#include "site.h"
#include "machine.h"

#include "praatP.h"
#include "EditorM.h"

#define PAGE_HEIGHT  320.0
#define PAPER_TOP  12.0
#define TOP_MARGIN  0.8
#define PAPER_BOTTOM  (13.0 - (double) thePrinter. paperHeight / thePrinter. resolution)
#define BOTTOM_MARGIN  0.5
static double resolution;

static int prefs_font = Graphics_TIMES, prefs_fontSize = 12;
static int codeFont = Graphics_COURIER;

void HyperPage_prefs (void) {
	Resources_addInt ("HyperPage.font", & prefs_font);
	Resources_addInt ("HyperPage.fontSize", & prefs_fontSize);
}

/********** class HyperLink **********/

class_methods (HyperLink, Data)
class_methods_end

HyperLink HyperLink_create (const char *name, double x1, double x2, double y1, double y2) {
	HyperLink me = new (HyperLink);
	if (! me) return NULL;
	Thing_setName (me, name);
	my x1 = x1, my x2 = x2, my y1 = y1, my y2 = y2;
	return me;
}

static void saveHistory (HyperPage me, const char *title) {
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
		if (strequ (my history [my historyPointer]. page, title)) return;
	} else if (my historyPointer > 0 && strequ (my history [my historyPointer - 1]. page, title)) {
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
	my history [my historyPointer]. page = Melder_strdup (title);
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
	char *leftHeader = reflect ? my outsideHeader : my insideHeader;
	char *rightHeader = reflect ? my insideHeader : my outsideHeader;
	char *leftFooter = reflect ? my outsideFooter : my insideFooter;
	char *rightFooter = reflect ? my insideFooter : my outsideFooter;
	my y = PAPER_TOP - TOP_MARGIN;
	my x = 0;
	my previousBottomSpacing = 0.0;
	Graphics_setFont (my ps, Graphics_TIMES);
	Graphics_setFontSize (my ps, 12);
	Graphics_setFontStyle (my ps, Graphics_ITALIC);
	if (leftHeader) {
		Graphics_setTextAlignment (my ps, Graphics_LEFT, Graphics_TOP);
		Graphics_printf (my ps, 0.7, PAPER_TOP, "%s", leftHeader);
	}
	if (my middleHeader) {
		Graphics_setTextAlignment (my ps, Graphics_CENTRE, Graphics_TOP);
		Graphics_printf (my ps, 0.7 + 3, PAPER_TOP, "%s", my middleHeader);
	}
	if (rightHeader) {
		Graphics_setTextAlignment (my ps, Graphics_RIGHT, Graphics_TOP);
		Graphics_printf (my ps, 0.7 + 6, PAPER_TOP, "%s", rightHeader);
	}
	if (leftFooter) {
		Graphics_setTextAlignment (my ps, Graphics_LEFT, Graphics_BOTTOM);
		Graphics_printf (my ps, 0.7, PAPER_BOTTOM, "%s", leftFooter);
	}
	if (my middleFooter) {
		Graphics_setTextAlignment (my ps, Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_printf (my ps, 0.7 + 3, PAPER_BOTTOM, "%s", my middleFooter);
	}
	if (rightFooter) {
		Graphics_setTextAlignment (my ps, Graphics_RIGHT, Graphics_BOTTOM);
		Graphics_printf (my ps, 0.7 + 6, PAPER_BOTTOM, "%s", rightFooter);
	}
	Graphics_setFontStyle (my ps, Graphics_NORMAL);
	if (my pageNumber)
		Graphics_printf (my ps, 0.7 + ( reflect ? 0 : 6 ), PAPER_BOTTOM, "%ld", my pageNumber);
	Graphics_setTextAlignment (my ps, Graphics_LEFT, Graphics_BOTTOM);
}

static void updateVerticalScrollBar (HyperPage me);

int HyperPage_any (I, const char *text, int font, int size, int style, double minFooterDistance,
	double x, double secondIndent, double topSpacing, double bottomSpacing, unsigned long method)
{
	iam (HyperPage);
	double heightGuess;
	heightGuess = size * (1.2/72) * ((long) size * strlen (text) / (int) (my rightMargin * 150));
if (! my printing) {
	Graphics_Link *paragraphLinks;
	int numberOfParagraphLinks, ilink;
	if (my entryHint && (method & HyperPage_USE_ENTRY_HINT) && strequ (text, my entryHint)) {
		my entryPosition = my y;
	}
	my y -= ( my previousBottomSpacing > topSpacing ? my previousBottomSpacing : topSpacing ) * size / 12.0;
	my y -= size * (1.2/72);
	my x = x;
	if (/* my y > PAGE_HEIGHT + 2.0 + heightGuess || */ my y < PAGE_HEIGHT - 10) {
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
	if (my y < PAPER_BOTTOM + BOTTOM_MARGIN + minFooterDistance + size * (1.2/72) * (strlen (text) / (6.0 * 10))) {
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

int HyperPage_pageTitle (I, const char *title) {
	iam (HyperPage);
	return HyperPage_any (me, title, my font, my fontSize * 2, 0,
		2.0, 0.0, 0.0, my printing ? 0.4/2 : 0.2/2, 0.3/2, HyperPage_ADD_BORDER);
}
int HyperPage_intro (I, const char *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.0, 0.03, 0.0, 0.1, 0.1, 0);
}
int HyperPage_entry (I, const char *title) {
	iam (HyperPage);
	return HyperPage_any (me, title, my font, my fontSize * 1.4, Graphics_BOLD, 0.5, 0.0, 0.0, 0.25/1.4, 0.1/1.4, HyperPage_USE_ENTRY_HINT);
}
int HyperPage_paragraph (I, const char *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.0, 0.03, 0.0, 0.1, 0.1, 0);
}
int HyperPage_listItem (I, const char *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.0, 0.30, 0.2, 0.0, 0.0, 0);
}
int HyperPage_listItem1 (I, const char *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.0, 0.57, 0.2, 0.0, 0.0, 0);
}
int HyperPage_listItem2 (I, const char *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.0, 0.84, 0.2, 0.0, 0.0, 0);
}
int HyperPage_listItem3 (I, const char *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.0, 1.11, 0.2, 0.0, 0.0, 0);
}
int HyperPage_listTag (I, const char *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.2, 0.03, 0.0, 0.1, 0.03, 0);
}
int HyperPage_listTag1 (I, const char *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.2, 0.50, 0.0, 0.05, 0.03, 0);
}
int HyperPage_listTag2 (I, const char *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.2, 0.97, 0.0, 0.03, 0.03, 0);
}
int HyperPage_listTag3 (I, const char *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.2, 1.44, 0.0, 0.03, 0.03, 0);
}
int HyperPage_definition (I, const char *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.0, 0.5, 0.0, 0.03, 0.1, 0);
}
int HyperPage_definition1 (I, const char *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.0, 0.97, 0.0, 0.03, 0.05, 0);
}
int HyperPage_definition2 (I, const char *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.0, 1.44, 0.0, 0.03, 0.03, 0);
}
int HyperPage_definition3 (I, const char *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.0, 1.93, 0.0, 0.03, 0.03, 0);
}
int HyperPage_code (I, const char *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, codeFont, my fontSize * 0.86, 0, 0.0, 0.3, 0.5, 0.0, 0.0, 0);
}
int HyperPage_code1 (I, const char *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, codeFont, my fontSize * 0.86, 0, 0.0, 0.6, 0.5, 0.0, 0.0, 0);
}
int HyperPage_code2 (I, const char *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, codeFont, my fontSize * 0.86, 0, 0.0, 0.9, 0.5, 0.0, 0.0, 0);
}
int HyperPage_code3 (I, const char *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, codeFont, my fontSize * 0.86, 0, 0.0, 1.2, 0.5, 0.0, 0.0, 0);
}
int HyperPage_code4 (I, const char *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, codeFont, my fontSize * 0.86, 0, 0.0, 1.5, 0.5, 0.0, 0.0, 0);
}
int HyperPage_code5 (I, const char *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, codeFont, my fontSize * 0.86, 0, 0.0, 1.8, 0.5, 0.0, 0.0, 0);
}
int HyperPage_prototype (I, const char *text) {
	iam (HyperPage);
	return HyperPage_any (me, text, my font, my fontSize, 0, 0.0, 0.03, 0.5, 0.0, 0.0, 0);
}
int HyperPage_formula (I, const char *formula) {
	iam (HyperPage);
	double topSpacing = 0.2, bottomSpacing = 0.2, minFooterDistance = 0.0;
	int font = my font, size = my fontSize;
if (! my printing) {
	my y -= ( my previousBottomSpacing > topSpacing ? my previousBottomSpacing : topSpacing ) * size / 12.0;
	my y -= size * (1.2/72);
	if (my y > PAGE_HEIGHT + 2.0 || my y < PAGE_HEIGHT - 10) {
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
	if (my y > PAGE_HEIGHT + height_inches || my y < PAGE_HEIGHT - 10) {
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

int HyperPage_script (I, double width_inches, double height_inches, const char *script) {
	iam (HyperPage);
	char *text = Melder_strdup (script);
	Interpreter interpreter = Interpreter_createFromEnvironment (NULL);
	double topSpacing = 0.1, bottomSpacing = 0.1, minFooterDistance = 0.0;
	int font = my font, size = my fontSize;
	width_inches *= width_inches < 0.0 ? -1.0 : size / 12.0;
	height_inches *= height_inches < 0.0 ? -1.0 : size / 12.0;
if (! my printing) {
	my y -= ( my previousBottomSpacing > topSpacing ? my previousBottomSpacing : topSpacing ) * size / 12.0;
	if (my y > PAGE_HEIGHT + height_inches || my y < PAGE_HEIGHT - 10) {
		my y -= height_inches;
	} else {
		my y -= height_inches;
		Graphics_setFont (my g, font);
		Graphics_setFontStyle (my g, 0);
		Graphics_setFontSize (my g, size);
		my x = width_inches > my rightMargin ? 0 : 0.5 * (my rightMargin - width_inches);
		Graphics_setWrapWidth (my g, 0);
		Graphics_setViewport (my g, my x, my x + width_inches, my y, my y + height_inches);
		{
			Graphics saveGraphics = praat.graphics;
			praat.graphics = my g;
			praatP.inManual = TRUE;
			praat_background ();
			Interpreter_run (interpreter, text);
			iferror Melder_clearError ();
			praat_foreground ();
			praatP.inManual = FALSE;
			praat.graphics = saveGraphics;
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
	Graphics_setViewport (my ps, my x, my x + width_inches, my y, my y + height_inches);
	{
		Graphics saveGraphics = praat.graphics;
		praat.graphics = my ps;
		praat_background ();
		Interpreter_run (interpreter, text);
		iferror Melder_clearError ();
		praat_foreground ();
		praat.graphics = saveGraphics;
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

static void draw (Any hyperPage) {
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
	int i;
	forget (my links);
	Melder_free (my entryHint);
	forget (my g);
	for (i = 0; i < 20; i ++) Melder_free (my history [i]. page);
	Melder_free (my currentPageTitle);
	inherited (HyperPage) destroy (me);
}

MOTIF_CALLBACK (cb_draw)
	iam (HyperPage);
#if defined (UNIX)
	if (((XmDrawingAreaCallbackStruct *) call) -> event -> xexpose. count) return;
#endif
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
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_input)
	iam (HyperPage);
	MotifEvent event = MotifEvent_fromCallData (call);
	int x = MotifEvent_x (event), y = MotifEvent_y (event), ilink;
	double xWC, yWC;
	if (! MotifEvent_isButtonPressedEvent (event)) return;
	Graphics_DCtoWC (my g, x, y, & xWC, & yWC);
	if (! my links) return;
	for (ilink = 1; ilink <= my links -> size; ilink ++) {
		HyperLink link = my links -> item [ilink];
		if (yWC > link -> y1 && yWC < link -> y2 && xWC > link -> x1 && xWC < link -> x2) {
			saveHistory (me, my currentPageTitle);
			if (! HyperPage_goToPage (me, link -> name)) {
				/* Allow for a returned 0 just to mean: 'do not jump'. */
				if (Melder_hasError ()) Melder_flushError (NULL);
			}
			return;
		}
	}
MOTIF_CALLBACK_END

DIRECT (HyperPage, cb_postScriptSettings)
	Printer_postScriptSettings ();
END

#ifdef macintosh
	DIRECT (HyperPage, cb_pageSetup)
		Printer_pageSetup ();
	END
#endif

FORM (HyperPage, cb_print, "Print", 0)
	SENTENCE ("Left or inside header", "")
	SENTENCE ("Middle header", "")
	LABEL ("", "Right or outside header:")
	TEXTFIELD ("Right or outside header", "")
	SENTENCE ("Left or inside footer", "")
	SENTENCE ("Middle footer", "")
	SENTENCE ("Right or outside footer", "")
	BOOLEAN ("Mirror even/odd headers", TRUE)
	INTEGER ("First page number", "0 (= no page numbers)")
	OK
our defaultHeaders (cmd);
if (my pageNumber) SET_INTEGER ("First page number", my pageNumber + 1)
DO
	my insideHeader = GET_STRING ("Left or inside header");
	my middleHeader = GET_STRING ("Middle header");
	my outsideHeader = GET_STRING ("Right or outside header");
	my insideFooter = GET_STRING ("Left or inside footer");
	my middleFooter = GET_STRING ("Middle footer");
	my outsideFooter = GET_STRING ("Right or outside footer");
	my mirror = GET_INTEGER ("Mirror even/odd headers");
	my pageNumber = GET_INTEGER ("First page number");
	Printer_print (print, me);
END

FORM (HyperPage, cb_font, "Font", 0)
	RADIO ("Font", 1)
		RADIOBUTTON ("Times")
		RADIOBUTTON ("Helvetica")
		RADIOBUTTON ("New Century Schoolbook")
		RADIOBUTTON ("Palatino")
	OK
SET_INTEGER ("Font", my font == Graphics_TIMES ? 1 :
		my font == Graphics_HELVETICA ? 2 : my font == Graphics_NEWCENTURYSCHOOLBOOK ? 3 : 4);
DO
	int font = GET_INTEGER ("Font");
	prefs_font = my font = font == 1 ? Graphics_TIMES : font == 2 ? Graphics_HELVETICA :
		font == 3 ? Graphics_NEWCENTURYSCHOOLBOOK : Graphics_PALATINO;
	if (my g) Graphics_updateWs (my g);
END

static void updateSizeMenu (HyperPage me) {
	XmToggleButtonGadgetSetState (my fontSizeButton_10, my fontSize == 10, 0);
	XmToggleButtonGadgetSetState (my fontSizeButton_12, my fontSize == 12, 0);
	XmToggleButtonGadgetSetState (my fontSizeButton_14, my fontSize == 14, 0);
	XmToggleButtonGadgetSetState (my fontSizeButton_18, my fontSize == 18, 0);
	XmToggleButtonGadgetSetState (my fontSizeButton_24, my fontSize == 24, 0);
}
static void setFontSize (HyperPage me, int fontSize) {
	prefs_fontSize = my fontSize = fontSize;
	if (my g) Graphics_updateWs (my g);
	updateSizeMenu (me);
}

DIRECT (HyperPage, cb_10) setFontSize (me, 10); END
DIRECT (HyperPage, cb_12) setFontSize (me, 12); END
DIRECT (HyperPage, cb_14) setFontSize (me, 14); END
DIRECT (HyperPage, cb_18) setFontSize (me, 18); END
DIRECT (HyperPage, cb_24) setFontSize (me, 24); END

FORM (HyperPage, cb_fontSize, "Font size", 0)
	NATURAL ("Font size (points)", "12")
	OK
SET_INTEGER ("Font size", my fontSize)
DO
	setFontSize (me, GET_INTEGER ("Font size"));
END

FORM (HyperPage, cb_searchForPage, "Search for page", 0)
	TEXTFIELD ("Page", "a")
	OK
DO
	if (! HyperPage_goToPage (me, GET_STRING ("Page"))) return 0;
END

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

static void createVerticalScrollBar (HyperPage me, Widget parent) {
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
		0);
}

static void updateVerticalScrollBar (HyperPage me)
/* We cannot call this immediately after creation. */
/* This has to be called after changing 'my topParagraph'. */
{
	Dimension width, height, marginWidth, marginHeight;
	int sliderSize;
	XtVaGetValues (my drawingArea, XmNwidth, & width, XmNheight, & height,
		XmNmarginWidth, & marginWidth, XmNmarginHeight, & marginHeight, NULL);
	sliderSize = 25 /*height / resolution * 5*/;   /* Don't change slider unless you clip value! */
	XmScrollBarSetValues (my verticalScrollBar, my top, sliderSize, 1, sliderSize - 1, False);
	my history [my historyPointer]. top = 0/*my top*/;
}

MOTIF_CALLBACK (cb_verticalScroll)
	iam (HyperPage);
	int value, sliderSize, incr, pincr;
	XmScrollBarGetValues (w, & value, & sliderSize, & incr, & pincr);
	if (value != my top) {
		my top = value;
		Graphics_clearWs (my g);
		initScreen (me);
		our draw (me);   /* Do not wait for expose event. */
		updateVerticalScrollBar (me);
	}
MOTIF_CALLBACK_END

DIRECT (HyperPage, cb_pageUp)
	int value, sliderSize, incr, pincr;
	if (! my verticalScrollBar) return 0;
	XmScrollBarGetValues (my verticalScrollBar, & value, & sliderSize, & incr, & pincr);
	value -= pincr;
	if (value < 0) value = 0;
	if (value != my top) {
		my top = value;
		Graphics_clearWs (my g);
		initScreen (me);
		our draw (me);   /* Do not wait for expose event. */
		updateVerticalScrollBar (me);
	}
END

DIRECT (HyperPage, cb_pageDown)
	int value, sliderSize, incr, pincr;
	if (! my verticalScrollBar) return 0;
	XmScrollBarGetValues (my verticalScrollBar, & value, & sliderSize, & incr, & pincr);
	value += pincr;
	if (value > (int) (PAGE_HEIGHT * 5) - sliderSize) value = (int) (PAGE_HEIGHT * 5) - sliderSize;
	if (value != my top) {
		my top = value;
		Graphics_clearWs (my g);
		initScreen (me);
		our draw (me);   /* Do not wait for expose event. */
		updateVerticalScrollBar (me);
	}
END

/********** **********/

static int do_back (HyperPage me) {
	char *page;
	int top;
	if (my historyPointer <= 0) return 1;
	page = Melder_strdup (my history [-- my historyPointer]. page);   /* Temporary, because pointer will be moved. */
	top = my history [my historyPointer]. top;
	if (our goToPage (me, page)) {
		my top = top;
		HyperPage_clear (me);
		updateVerticalScrollBar (me);
	} else return 0;
	Melder_free (page);
	return 1;
}

DIRECT (HyperPage, cb_back)
	if (! do_back (me)) return 0;
END

MOTIF_CALLBACK (cb_backButton)
	iam (HyperPage);
	if (! do_back (me)) Melder_flushError (NULL);
MOTIF_CALLBACK_END

static int do_forth (HyperPage me) {
	char *page;
	int top;
	if (my historyPointer >= 19 || ! my history [my historyPointer + 1]. page) return 1;
	page = Melder_strdup (my history [++ my historyPointer]. page);
	top = my history [my historyPointer]. top;
	if (our goToPage (me, page)) {
		my top = top;
		HyperPage_clear (me);
		updateVerticalScrollBar (me);
	} else return 0;
	Melder_free (page);
	return 1;
}

DIRECT (HyperPage, cb_forth)
	if (! do_forth (me)) return 0;
END

MOTIF_CALLBACK (cb_forthButton)
	iam (HyperPage);
	if (! do_forth (me)) Melder_flushError (NULL);
MOTIF_CALLBACK_END

static void createMenus (I) {
	iam (HyperPage);
	inherited (HyperPage) createMenus (me);

	Editor_addCommand (me, "File", "PostScript settings...", 0, cb_postScriptSettings);
	#ifdef macintosh
		Editor_addCommand (me, "File", "Page setup...", 0, cb_pageSetup);
	#endif
	Editor_addCommand (me, "File", "Print page...", 'P', cb_print);
	Editor_addCommand (me, "File", "-- close --", 0, NULL);

	if (our hasHistory) {
		Editor_addMenu (me, "Go to", 0);
		Editor_addCommand (me, "Go to", "Search for page...", 0, cb_searchForPage);
		Editor_addCommand (me, "Go to", "Back", motif_OPTION | motif_LEFT_ARROW, cb_back);
		Editor_addCommand (me, "Go to", "Forward", motif_OPTION | motif_RIGHT_ARROW, cb_forth);
		Editor_addCommand (me, "Go to", "-- page --", 0, NULL);
		Editor_addCommand (me, "Go to", "Page up", motif_PAGE_UP, cb_pageUp);
		Editor_addCommand (me, "Go to", "Page down", motif_PAGE_DOWN, cb_pageDown);
	}

	Editor_addMenu (me, "Font", 0);
	my fontSizeButton_10 = Editor_addCommand (me, "Font", "10", motif_CHECKABLE, cb_10);
	my fontSizeButton_12 = Editor_addCommand (me, "Font", "12", motif_CHECKABLE, cb_12);
	my fontSizeButton_14 = Editor_addCommand (me, "Font", "14", motif_CHECKABLE, cb_14);
	my fontSizeButton_18 = Editor_addCommand (me, "Font", "18", motif_CHECKABLE, cb_18);
	my fontSizeButton_24 = Editor_addCommand (me, "Font", "24", motif_CHECKABLE, cb_24);
	Editor_addCommand (me, "Font", "Font size...", 0, cb_fontSize);
	Editor_addCommand (me, "Font", "Font...", 0, cb_font);
}

/********** **********/

MOTIF_CALLBACK (cb_resize)
	iam (HyperPage);
	Dimension width, height, marginWidth, marginHeight;
	XtVaGetValues (w, XmNwidth, & width, XmNheight, & height,
		XmNmarginWidth, & marginWidth, XmNmarginHeight, & marginHeight, NULL);
	Graphics_setWsViewport (my g, marginWidth, width - marginWidth,
		marginHeight, height - marginHeight);
	Graphics_setWsWindow (my g, 0.0, my rightMargin = (width - 2 * marginWidth) / resolution,
		PAGE_HEIGHT - (height - 2 * marginHeight) / resolution, PAGE_HEIGHT);
	if (my g) Graphics_updateWs (my g);
	updateVerticalScrollBar (me);
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_previousPage)
	iam (HyperPage);
	HyperPage_goToPage_i (me, our getCurrentPageNumber (me) > 1 ?
		our getCurrentPageNumber (me) - 1 : our getNumberOfPages (me));
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_nextPage)
	iam (HyperPage);
	HyperPage_goToPage_i (me, our getCurrentPageNumber (me) < our getNumberOfPages (me) ?
		our getCurrentPageNumber (me) + 1 : 1);
MOTIF_CALLBACK_END

static void createChildren (I) {
	iam (HyperPage);
	Widget button;
	int height = Machine_getTextHeight ();
	int y = Machine_getMenuBarHeight () + 4;

	/***** Create navigation buttons. *****/

	if (our hasHistory) {
		button = XtVaCreateManagedWidget ("<", xmPushButtonWidgetClass, my dialog,
			XmNx, 4, XmNy, y, XmNheight, height, XmNwidth, 44, 0);
		XtAddCallback (button, XmNactivateCallback, cb_backButton, (XtPointer) me);
		button = XtVaCreateManagedWidget (">", xmPushButtonWidgetClass, my dialog,
			XmNx, 54, XmNy, y, XmNheight, height, XmNwidth, 44, 0);
		XtAddCallback (button, XmNactivateCallback, cb_forthButton, (XtPointer) me);
	}
	if (our isOrdered) {
		button = XtVaCreateManagedWidget ("< 1", xmPushButtonWidgetClass, my dialog,
			XmNx, 174, XmNy, y, XmNheight, height, XmNwidth, 44, 0);
		XtAddCallback (button, XmNactivateCallback, cb_previousPage, (XtPointer) me);
		button = XtVaCreateManagedWidget ("1 >", xmPushButtonWidgetClass, my dialog,
			XmNx, 224, XmNy, y, XmNheight, height, XmNwidth, 44, 0);
		XtAddCallback (button, XmNactivateCallback, cb_nextPage, (XtPointer) me);
	}

	/***** Create scroll bar. *****/

	createVerticalScrollBar (me, my dialog);

	/***** Create drawing area. *****/

	my drawingArea = XmCreateDrawingArea (my dialog, "drawingArea", NULL, 0);
	XtVaSetValues (my drawingArea,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, Machine_getScrollBarWidth (),
		XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, y + height + 8,
		XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, Machine_getScrollBarWidth (),
		XmNmarginWidth, 20, XmNborderWidth, 1,
		0);
	XtManageChild (my drawingArea);
}

int HyperPage_init (I, Widget parent, const char *title, Any data) {
	iam (HyperPage);
	#if defined (UNIX)
		Display *display = XtDisplay (parent);
		resolution = floor (25.4 * (double) DisplayWidth (display, DefaultScreen (display)) /
			DisplayWidthMM (display, DefaultScreen (display)) + 0.5);
	#else
		resolution = 72;
	#endif
	if (! Editor_init (me, parent, 0, 0, 6 * resolution + 30, 800, title, data)) { forget (me); return 0; }
	Melder_assert (XtWindow (my drawingArea));
	my g = Graphics_create_xmdrawingarea (my drawingArea);
	Graphics_setAtSignIsLink (my g, TRUE);
	Graphics_setDollarSignIsCode (my g, TRUE);
	Graphics_setFont (my g, Graphics_TIMES);
	my font = prefs_font;
	setFontSize (me, prefs_fontSize);
	XtAddCallback (my drawingArea, XmNexposeCallback, cb_draw, (XtPointer) me);
	XtAddCallback (my drawingArea, XmNinputCallback, cb_input, (XtPointer) me);
	XtAddCallback (my drawingArea, XmNresizeCallback, cb_resize, (XtPointer) me);
	cb_resize (my drawingArea, (XtPointer) me, NULL);   /* Force WsWindow. */
	XtAddCallback (my verticalScrollBar, XmNvalueChangedCallback, cb_verticalScroll, (XtPointer) me);
	XtAddCallback (my verticalScrollBar, XmNdragCallback, cb_verticalScroll, (XtPointer) me);
	updateVerticalScrollBar (me);   /* Scroll to the top (my top == 0). */
	return 1;
}

void HyperPage_clear (HyperPage me) {
	Graphics_updateWs (my g);
	forget (my links);
}

static void dataChanged (I) {
	iam (HyperPage);
	int oldError = Melder_hasError ();
	(void) our goToPage (me, my currentPageTitle);
	if (Melder_hasError () && ! oldError) Melder_flushError (NULL);
	HyperPage_clear (me);
	updateVerticalScrollBar (me);
}
static long getNumberOfPages (Any hyperPage) {
	(void) hyperPage;
	return 0;
}
static long getCurrentPageNumber (Any hyperPage) {
	(void) hyperPage;
	return 0;
}
static void defaultHeaders (EditorCommand cmd) {
	(void) cmd;
}
static int goToPage (Any hyperPage, const char *title) {
	(void) hyperPage;
	(void) title;
	return 0;
}
static int goToPage_i (Any hyperPage, long i) {
	(void) hyperPage;
	(void) i;
	return 0;
}
static int hasHistory = FALSE;
static int isOrdered = FALSE;

class_methods (HyperPage, Editor)
	class_method (destroy)
	class_method (dataChanged)
	class_method (draw)
	us -> editable = FALSE;
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

int HyperPage_goToPage (I, const char *title) {
	iam (HyperPage);
	switch (our goToPage (me, title)) {
		case -1: return 0;
		case 0: HyperPage_clear (me); return 0;
	}
	saveHistory (me, title);   /* Last chance: HyperPage_clear will destroy "title" !!! */
	Melder_free (my currentPageTitle);
	my currentPageTitle = Melder_strdup (title);
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

void HyperPage_setEntryHint (I, const char *hint) {
	iam (HyperPage);
	Melder_free (my entryHint);
	my entryHint = Melder_strdup (hint);
}

/* End of file HyperPage.c */
