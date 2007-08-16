/* Manual.c
 *
 * Copyright (C) 1996-2007 Paul Boersma
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
 * pb 2003/03/09 searching: more points for multiple occurrences within a paragraph
 * pb 2003/10/03 praat_executeFromFile without arguments
 * pb 2003/11/26 use recording time from file
 * pb 2003/11/30 removed newline from date
 * pb 2004/02/08 allow arguments in scripts
 * pb 2005/05/08 embedded scripts (for pictures)
 * pb 2005/07/19 moved navigation buttons to the top, removed page label and horizontal scroll bar
 * pb 2006/10/20 embedded scripts: not on opening page
 * pb 2007/06/10 wchar_t
 * pb 2007/08/12 wchar_t
 */

#include <ctype.h>
#include "Manual.h"
#include "Printer.h"
#include "machine.h"
#include "site.h"
#include <time.h>
#include "EditorM.h"
#include "praat_script.h"

/* Remaining BUGS: HTML writer does not recognize "\s{". */

#define SEARCH_PAGE  0

static const wchar_t *month [] =
	{ L"", L"January", L"February", L"March", L"April", L"May", L"June",
	  L"July", L"August", L"September", L"October", L"November", L"December" };

FORM_WRITE (Manual, cb_writeOneToHtmlFile, "Write to HTML file", 0)
	ManPages manPages = my data;
	wchar_t *p = defaultName;
	wcscpy (p, ((ManPage) manPages -> pages -> item [my path]) -> title);
	while (*p) { if (! isalnum (*p) && *p != '_') *p = '_'; p ++; }
	wcscat (defaultName, L".html");
DO_WRITE
	if (! ManPages_writeOneToHtmlFile (my data, my path, file)) return 0;
END

FORM (Manual, cb_writeAllToHtmlDir, "Write all pages as HTML files", 0)
	LABEL ("", "Type a directory name:")
	TEXTFIELD ("directory", "")
	OK
structMelderDir currentDirectory = { { 0 } };
Melder_getDefaultDir (& currentDirectory);
SET_STRING ("directory", Melder_dirToPath (& currentDirectory))
DO
	wchar_t *directory = GET_STRINGW (L"directory");
	if (! ManPages_writeAllToHtmlDir (my data, directory)) return 0;
END

FORM (Manual, cb_searchForPageList, "Search for page", 0)
	{ ManPages manPages = my data;
	long numberOfPages;
	const wchar_t **pages = ManPages_getTitles (manPages, & numberOfPages);
	LIST ("Page", manPages -> pages -> size, pages, 1) }
	OK
DO
	if (! HyperPage_goToPage_i (me, GET_INTEGER ("Page"))) return 0;
END

static void destroy (I) {
	iam (Manual);
	ManPages pages = my data;
	if (pages && pages -> dynamic) forget (pages);
	inherited (Manual) destroy (me);
}

static void draw (I) {
	iam (Manual);
	ManPages manPages = my data;
	ManPage page;
	ManPage_Paragraph paragraph;
	Graphics_clearWs (my g);
	if (my path == SEARCH_PAGE) {
		int i;
		HyperPage_pageTitle (me, L"Best matches");
		HyperPage_intro (me, L"The best matches to your query seem to be:");
		for (i = 1; i <= my numberOfMatches; i ++) {
			wchar_t link [300];
			page = manPages -> pages -> item [my matches [i]];
			swprintf (link, 300, L"\\bu @@%ls", page -> title);
			HyperPage_listItem (me, link);
		}
		return;
	}
	page = manPages -> pages -> item [my path];
	if (! my paragraphs) return;
	HyperPage_pageTitle (me, page -> title);
	for (paragraph = & page -> paragraphs [0]; paragraph -> type != 0; paragraph ++) {
		switch (paragraph -> type) {
			case  enumi (ManPage_TYPE, intro): HyperPage_intro (me, paragraph -> text); break;
			case  enumi (ManPage_TYPE, entry): HyperPage_entry (me, paragraph -> text); break;
			case  enumi (ManPage_TYPE, normal): HyperPage_paragraph (me, paragraph -> text); break;
			case  enumi (ManPage_TYPE, list_item): HyperPage_listItem (me, paragraph -> text); break;
			case  enumi (ManPage_TYPE, tag): HyperPage_listTag (me, paragraph -> text); break;
			case  enumi (ManPage_TYPE, definition): HyperPage_definition (me, paragraph -> text); break;
			case  enumi (ManPage_TYPE, code): HyperPage_code (me, paragraph -> text); break;
			case  enumi (ManPage_TYPE, prototype): HyperPage_prototype (me, paragraph -> text); break;
			case  enumi (ManPage_TYPE, formula): HyperPage_formula (me, paragraph -> text); break;
			case  enumi (ManPage_TYPE, picture): HyperPage_picture (me, paragraph -> width,
				paragraph -> height, paragraph -> draw); break;
			case  enumi (ManPage_TYPE, script): HyperPage_script (me, paragraph -> width,
				paragraph -> height, paragraph -> text); break;
			case  enumi (ManPage_TYPE, list_item1): HyperPage_listItem1 (me, paragraph -> text); break;
			case  enumi (ManPage_TYPE, list_item2): HyperPage_listItem2 (me, paragraph -> text); break;
			case  enumi (ManPage_TYPE, list_item3): HyperPage_listItem3 (me, paragraph -> text); break;
			case  enumi (ManPage_TYPE, tag1): HyperPage_listTag1 (me, paragraph -> text); break;
			case  enumi (ManPage_TYPE, tag2): HyperPage_listTag2 (me, paragraph -> text); break;
			case  enumi (ManPage_TYPE, tag3): HyperPage_listTag3 (me, paragraph -> text); break;
			case  enumi (ManPage_TYPE, definition1): HyperPage_definition1 (me, paragraph -> text); break;
			case  enumi (ManPage_TYPE, definition2): HyperPage_definition2 (me, paragraph -> text); break;
			case  enumi (ManPage_TYPE, definition3): HyperPage_definition3 (me, paragraph -> text); break;
			case  enumi (ManPage_TYPE, code1): HyperPage_code1 (me, paragraph -> text); break;
			case  enumi (ManPage_TYPE, code2): HyperPage_code2 (me, paragraph -> text); break;
			case  enumi (ManPage_TYPE, code3): HyperPage_code3 (me, paragraph -> text); break;
			case  enumi (ManPage_TYPE, code4): HyperPage_code4 (me, paragraph -> text); break;
			case  enumi (ManPage_TYPE, code5): HyperPage_code5 (me, paragraph -> text); break;
			default: break;
		}
	}
	if (ManPages_uniqueLinksHither (manPages, my path)) {
		long ilink, jlink, lastParagraph = 0;
		int goAhead = TRUE;
		while (page -> paragraphs [lastParagraph]. type != 0) lastParagraph ++;
		if (lastParagraph > 0) {
			const wchar_t *text = page -> paragraphs [lastParagraph - 1]. text;
			if (text == NULL || text [0] == '\0' || text [wcslen (text) - 1] != ':') {
				if (my printing && my suppressLinksHither)
					goAhead = FALSE;
				else
					HyperPage_entry (me, L"Links to this page");
			}
		}
		if (goAhead) for (ilink = 1; ilink <= page -> nlinksHither; ilink ++) {
			long link = page -> linksHither [ilink];
			int alreadyShown = FALSE;
			for (jlink = 1; jlink <= page -> nlinksThither; jlink ++)
				if (page -> linksThither [jlink] == link)
					alreadyShown = TRUE;
			if (! alreadyShown) {
				const wchar_t *title = ((ManPage) manPages -> pages -> item [page -> linksHither [ilink]]) -> title;
				wchar_t linkText [304];
				swprintf (linkText, 304, L"@@%ls@", title);
				HyperPage_listItem (me, linkText);
			}
		}
	}
	if (! my printing && page -> date) {
		wchar_t signature [100];
		long date = page -> date;
		int imonth = date % 10000 / 100;
		if (imonth < 0 || imonth > 12) imonth = 0;
		swprintf (signature, 100, L"\\co %ls, %ls %ld, %ld",
			wcsequ (page -> author, L"ppgb") ? L"Paul Boersma" :
			wcsequ (page -> author, L"djmw") ? L"David Weenink" : page -> author,
			month [imonth], date % 100, date / 10000);
		HyperPage_any (me, L"", my font, my fontSize, 0, 0.0,
			0.0, 0.0, 0.1, 0.1, HyperPage_ADD_BORDER);
		HyperPage_any (me, signature, my font, my fontSize, Graphics_ITALIC, 0.0,
			0.03, 0.0, 0.1, 0.0, 0);
	}
}

/********** PRINTING **********/

static void print (I, Graphics graphics) {
	iam (Manual);
	ManPages manPages = my data;
	int numberOfPages = manPages -> pages -> size, savePage = my path, ipage;
	my ps = graphics;
	Graphics_setDollarSignIsCode (my ps, TRUE);
	Graphics_setAtSignIsLink (my ps, TRUE);
	my printing = TRUE;
	HyperPage_initSheetOfPaper ((HyperPage) me);
	for (ipage = 1; ipage <= numberOfPages; ipage ++) {
		ManPage page = manPages -> pages -> item [ipage];
		if (my printPagesStartingWith == NULL ||
		    Melder_stringMatchesCriterion (page -> title, Melder_STRING_STARTS_WITH, my printPagesStartingWith))
		{
			ManPage_Paragraph par;
			my path = ipage;
			my paragraphs = page -> paragraphs;
			my numberOfParagraphs = 0;
			par = my paragraphs;
			while ((par ++) -> type) my numberOfParagraphs ++;
			Melder_free (my currentPageTitle);
			my currentPageTitle = Melder_wcsdup (page -> title);
			our goToPage_i (me, ipage);
			our draw (me);
			our goToPage_i (me, savePage);
		}
	}
	my printing = FALSE;
	my printPagesStartingWith = NULL;
}

FORM (Manual, cb_printRange, "Print range", 0)
	SENTENCE ("Left or inside header", "")
	SENTENCE ("Middle header", "")
	SENTENCE ("Right or outside header", "Manual")
	SENTENCE ("Left or inside footer", "")
	SENTENCE ("Middle footer", "")
	SENTENCE ("Right or outside footer", "")
	BOOLEAN ("Mirror even/odd headers", TRUE)
	LABEL ("", "Print all pages whose title starts with:")
	TEXTFIELD ("Print pages starting with", "Intro")
	INTEGER ("First page number", "1")
	BOOLEAN ("Suppress \"Links to this page\"", FALSE)
	OK
ManPages manPages = my data;
time_t today = time (NULL);
char date [50], *newline;
#ifdef UNIX
	struct tm *tm = localtime (& today);
	strftime (date, 50, "%B %e, %Y", tm);
#else
	strcpy (date, ctime (& today));
#endif
newline = strchr (date, '\n'); if (newline) *newline = '\0';
SET_STRING ("Left or inside header", date)
SET_STRING ("Right or outside header", my name)
if (my pageNumber) SET_INTEGER ("First page number", my pageNumber + 1)
if (my path >= 1 && my path <= manPages -> pages -> size) {
	ManPage page = manPages -> pages -> item [my path];
	SET_STRINGW (L"Print pages starting with", page -> title);
}
DO
	my insideHeader = GET_STRINGW (L"Left or inside header");
	my middleHeader = GET_STRINGW (L"Middle header");
	my outsideHeader = GET_STRINGW (L"Right or outside header");
	my insideFooter = GET_STRINGW (L"Left or inside footer");
	my middleFooter = GET_STRINGW (L"Middle footer");
	my outsideFooter = GET_STRINGW (L"Right or outside footer");
	my mirror = GET_INTEGER ("Mirror even/odd headers");
	my printPagesStartingWith = GET_STRINGW (L"Print pages starting with");
	my pageNumber = GET_INTEGER ("First page number");
	my suppressLinksHither = GET_INTEGER ("Suppress \"Links to this page\"");
	Printer_print (print, me);
END

/********** SEARCHING **********/

static float *goodnessOfMatch;

static float searchToken (ManPages me, long ipage, wchar_t *token) {
	double goodness = 0.0;
	ManPage page = my pages -> item [ipage];
	struct structManPage_Paragraph *par = & page -> paragraphs [0];
	if (! token [0]) return 1.0;
	/*
	 * Try to find a match in the title, case insensitively.
	 */
	static MelderStringW buffer = { 0 };
	MelderStringW_copyW (& buffer, page -> title);
	for (wchar_t *p = & buffer.string [0]; *p != '\0'; p ++) *p = tolower (*p);
	if (wcsstr (buffer.string, token)) {
		goodness += 300.0;   /* Lots of points for a match in the title! */
		if (wcsequ (buffer.string, token))
			goodness += 10000.0;   /* Even more points for an exact match! */
	}
	/*
	 * Try to find a match in the paragraphs, case-insensitively.
	 */
	while (par -> type) {
		if (par -> text) {
			wchar_t *ptoken;
			MelderStringW_copyW (& buffer, par -> text);
			for (wchar_t *p = & buffer.string [0]; *p != '\0'; p ++) *p = tolower (*p);
			ptoken = wcsstr (buffer.string, token);
			if (ptoken) {
				goodness += 10.0;   /* Ten points for every paragraph with a match! */
				if (wcsstr (ptoken + wcslen (token), token)) {
					goodness += 1.0;   /* One point for every second occurrence in a paragraph! */
				}
			}
		}
		par ++;
	}
	return goodness;
}

static void search (Manual me, const wchar_t *query) {
	ManPages manPages = my data;
	long numberOfPages = manPages -> pages -> size, ipage, imatch;
	static MelderStringW searchText = { 0 };
	wchar_t *p;
	MelderStringW_copyW (& searchText, query);
	for (p = & searchText.string [0]; *p != '\0'; p ++) {
		if (*p == '\n') *p = ' ';
		*p = tolower (*p);
	}
	if (! goodnessOfMatch && ! (goodnessOfMatch = NUMfvector (1, numberOfPages)))
		{ Melder_flushError (NULL); return; }
	for (ipage = 1; ipage <= numberOfPages; ipage ++) {
		wchar_t *token = searchText.string;
		goodnessOfMatch [ipage] = 1.0;
		for (;;) {
			wchar_t *space = wcschr (token, ' ');
			if (space) *space = '\0';
			goodnessOfMatch [ipage] *= searchToken (manPages, ipage, token);
			if (! space) break;
			*space = ' ';   /* Restore. */
			token = space + 1;
		}
	}
	/*
	 * Find the 20 best matches.
	 */
	my numberOfMatches = 0;
	for (imatch = 1; imatch <= 20; imatch ++) {
		long imax = 0;
		float max = 0.0;
		for (ipage = 1; ipage <= numberOfPages; ipage ++) {
			if (goodnessOfMatch [ipage] > max) {
				max = goodnessOfMatch [ipage];
				imax = ipage;
			}
		}
		if (! imax) break;
		my matches [++ my numberOfMatches] = imax;
		goodnessOfMatch [imax] = 0.0;   /* Skip next time. */
	}
	if (! HyperPage_goToPage_i (me, SEARCH_PAGE)) Melder_flushError (NULL);
}

void Manual_search (Manual me, const wchar_t *query) {
	GuiText_setStringW (my searchText, query);
	search (me, query);
}

MOTIF_CALLBACK (cb_home)
	iam (Manual);
	ManPages pages = my data;
	long iHome = ManPages_lookUp (pages, L"Intro");
	HyperPage_goToPage_i (me, iHome ? iHome : 1);
MOTIF_CALLBACK_END
 
MOTIF_CALLBACK (cb_record)
	iam (Manual);
	ManPages manPages = my data;
	ManPage manPage = my path < 1 ? NULL : manPages -> pages -> item [my path];
	XtSetSensitive (my recordButton, False);
	XtSetSensitive (my playButton, False);
	XtSetSensitive (my publishButton, False);
	XmUpdateDisplay (my shell);
	if (! Melder_record (manPage == NULL ? 1.0 : manPage -> recordingTime)) Melder_flushError (NULL);
	XtSetSensitive (my recordButton, True);
	XtSetSensitive (my playButton, True);
	XtSetSensitive (my publishButton, True);
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_play)
	iam (Manual);
	XtSetSensitive (my recordButton, False);
	XtSetSensitive (my playButton, False);
	XtSetSensitive (my publishButton, False);
	XmUpdateDisplay (my shell);
	Melder_play ();
	XtSetSensitive (my recordButton, True);
	XtSetSensitive (my playButton, True);
	XtSetSensitive (my publishButton, True);
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_publish)
	Melder_publishPlayed ();
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_search)
	iam (Manual);
	wchar_t *query = GuiText_getStringW (my searchText);
	search (me, query);
	Melder_free (query);
MOTIF_CALLBACK_END

static void createChildren (I) {
	iam (Manual);
	ManPages pages = my data;   /* Has been installed here by Editor_init (). */
	Widget button;
#if defined (macintosh)
	#define STRING_SPACING 8
#else
	#define STRING_SPACING 2
#endif
	int height = Machine_getTextHeight (), y = Machine_getMenuBarHeight () + 4;
	inherited (Manual) createChildren (me);
	my homeButton = XtVaCreateManagedWidget ("Home", xmPushButtonWidgetClass, my dialog,
		XmNx, 104, XmNy, y, XmNheight, height, XmNwidth, 64, NULL);
	XtAddCallback (my homeButton, XmNactivateCallback, cb_home, (XtPointer) me);
	if (pages -> dynamic) {
		XtVaSetValues (my drawingArea, XmNtopOffset, y + height * 2 + 16, NULL);
		XtVaSetValues (my verticalScrollBar, XmNtopOffset, y + height * 2 + 16, NULL);
		my recordButton = XtVaCreateManagedWidget ("Record", xmPushButtonWidgetClass, my dialog,
			XmNx, 4, XmNy, y+height+8, XmNheight, height, XmNwidth, 75, NULL);
		XtAddCallback (my recordButton, XmNactivateCallback, cb_record, (XtPointer) me);
		my playButton = XtVaCreateManagedWidget ("Play", xmPushButtonWidgetClass, my dialog,
			XmNx, 85, XmNy, y+height+8, XmNheight, height, XmNwidth, 75, NULL);
		XtAddCallback (my playButton, XmNactivateCallback, cb_play, (XtPointer) me);
		my publishButton = XtVaCreateManagedWidget ("Copy last played to list", xmPushButtonWidgetClass, my dialog,
			XmNx, 166, XmNy, y+height+8, XmNheight, height, XmNwidth, 175, NULL);
		XtAddCallback (my publishButton, XmNactivateCallback, cb_publish, (XtPointer) me);
	}
	button = XtVaCreateManagedWidget ("Search:", xmPushButtonWidgetClass, my dialog,
		XmNx, 274, XmNy, y, XmNheight, height, XmNwidth, 63, NULL);
	XtAddCallback (button, XmNactivateCallback, cb_search, (XtPointer) me);
	#ifdef _WIN32
	/* BUG: activateCallback should work for texts. */
	XtVaSetValues (my dialog, XmNdefaultButton, button, NULL);
	#endif
	my searchText = XtVaCreateManagedWidget ("searchText", xmTextFieldWidgetClass, my dialog,
		XmNx, 274+63 + STRING_SPACING, XmNy, y, XmNwidth, 452 - (274+63) - 2, NULL);
	XtAddCallback (my searchText, XmNactivateCallback, cb_search, (XtPointer) me);
}

DIRECT (Manual, cb_help)
	if (! HyperPage_goToPage (me, L"Manual")) return 0;
END

static void createMenus (I) {
	iam (Manual);
	inherited (Manual) createMenus (me);

	Editor_addCommand (me, L"File", L"Print manual...", 0, cb_printRange);
	Editor_addCommand (me, L"File", L"Write page to HTML file...", 0, cb_writeOneToHtmlFile);
	Editor_addCommand (me, L"File", L"Write manual to HTML directory...", 0, cb_writeAllToHtmlDir);
	Editor_addCommand (me, L"File", L"-- close --", 0, NULL);

	Editor_addCommand (me, L"Go to", L"Search for page (list)...", 0, cb_searchForPageList);

	Editor_addCommand (me, L"Help", L"Manual help", '?', cb_help);
}

static void defaultHeaders (EditorCommand cmd) {
	Manual me = (Manual) cmd -> editor;
	ManPages manPages = my data;
	if (my path) {
		wchar_t string [400];
		static const wchar_t *month [] =
			{ L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun", L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec" };
		ManPage page = manPages -> pages -> item [my path];
		long date = page -> date;
		SET_STRINGW (L"Right or outside header", page -> title)
		SET_STRINGW (L"Left or inside footer", page -> author)
		if (date) {
			swprintf (string, 400, L"%ls %ld, %ld", month [date % 10000 / 100 - 1], date % 100, date / 10000);
			SET_STRINGW (L"Left or inside header", string)
		}
	}
}

static long getNumberOfPages (I) {
	iam (Manual);
	ManPages manPages = my data;
	return manPages -> pages -> size;
}

static long getCurrentPageNumber (I) {
	iam (Manual);
	return my path ? my path : 1;
}

static int goToPage_i (I, long i) {
	iam (Manual);
	ManPages manPages = my data;
	ManPage page;
	ManPage_Paragraph par;
	if (i < 1 || i > manPages -> pages -> size) {
		if (i == SEARCH_PAGE) {
			my path = SEARCH_PAGE;
			Melder_free (my currentPageTitle);
			return 1;
		} else return Melder_error ("Page %ld not found.", i);
	}
	my path = i;
	page = manPages -> pages -> item [my path];
	my paragraphs = page -> paragraphs;
	my numberOfParagraphs = 0;
	par = my paragraphs;
	while ((par ++) -> type) my numberOfParagraphs ++;
	Melder_free (my currentPageTitle);
	my currentPageTitle = Melder_wcsdup (page -> title);
	return 1;
}

static int goToPage (I, const wchar_t *title) {
	iam (Manual);
	ManPages manPages = my data;
	if (title [0] == '\\' && title [1] == 'F' && title [2] == 'I') {
		structMelderFile file = { 0 };
		MelderDir_relativePathToFile (& manPages -> rootDirectory, title + 3, & file);
		Melder_recordFromFile (& file);
		return -1;
	} else if (title [0] == '\\' && title [1] == 'S' && title [2] == 'C') {
		structMelderDir saveDir = { { 0 } };
		Melder_getDefaultDir (& saveDir);
		Melder_setDefaultDir (& manPages -> rootDirectory);
		if (! praat_executeScriptFromFileNameWithArguments (title + 3)) {
			Melder_flushError (NULL);
		}
		Melder_setDefaultDir (& saveDir);
		return 0;
	} else {
		long i;
		i = ManPages_lookUp (manPages, title);
		if (! i)
			return Melder_error3 (L"Page \"", title, L"\" not found.");
		return goToPage_i (me, i);
	}
}

static int hasHistory = TRUE, isOrdered = TRUE;

class_methods (Manual, HyperPage)
	class_method (destroy)
	us -> scriptable = FALSE;
	class_method (draw)
	class_method (createChildren)
	class_method (createMenus)
	class_method (defaultHeaders)
	class_method (getNumberOfPages)
	class_method (getCurrentPageNumber)
	class_method (goToPage)
	class_method (goToPage_i)
	class_method (hasHistory)
	class_method (isOrdered)
class_methods_end

int Manual_init (I, Widget parent, const wchar_t *title, Any data) {
	iam (Manual);
	ManPages manPages = data;
	wchar_t windowTitle [101];
	long i;
	ManPage page;
	ManPage_Paragraph par;
	if (! (i = ManPages_lookUp (manPages, title)))
		return Melder_error3 (L"Page \"", title, L"\" not found.");
	my path = i;
	page = manPages -> pages -> item [i];
	my paragraphs = page -> paragraphs;
	my numberOfParagraphs = 0;
	par = my paragraphs;
	while ((par ++) -> type) my numberOfParagraphs ++;

	if (((ManPage) manPages -> pages -> item [1]) -> title [0] == '-') {
		wcscpy (windowTitle, & ((ManPage) manPages -> pages -> item [1]) -> title [1]);
		if (windowTitle [wcslen (windowTitle) - 1] == '-') windowTitle [wcslen (windowTitle) - 1] = '\0';
	} else {
		wcscpy (windowTitle, L"Manual");
	}
	if (! HyperPage_init (me, parent, windowTitle, data)) { forget (me); return 0; }
	MelderDir_copy (& manPages -> rootDirectory, & my rootDirectory);
	my history [0]. page = Melder_wcsdup (title);   /* BAD */
	return 1;
}

Manual Manual_create (Widget parent, const wchar_t *title, Any data) {
	Manual me = new (Manual);
	if (! me || ! Manual_init (me, parent, title, data)) forget (me);
	return me;
}

/* End of file Manual.c */
